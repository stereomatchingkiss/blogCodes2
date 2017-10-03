#include "marble_slab.hpp"

#include <opencv2/calib3d.hpp>
#include <opencv2/core.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <iostream>

namespace {

void marble_slab_top_right()
{
    cv::Mat input = cv::imread("../forum_quest/data/complex_bg.jpg");
    if(input.empty()){
        std::cerr<<"cannot open image\n";
        return;
    }

    cv::Mat blur_img;
    cv::GaussianBlur(input, blur_img, {11, 11}, 7);
    cv::medianBlur(blur_img, blur_img, 3);

    cv::Mat edges;
    cv::Canny(blur_img, edges, 40, 90);

    //fill up missed line of the marble slab, you can try with another
    //operation like close
    cv::Mat const kernel = cv::getStructuringElement(cv::MORPH_RECT, {7, 7});
    cv::Mat morpho;
    cv::dilate(edges, morpho, kernel);

    cv::Mat drawing = cv::Mat::zeros(morpho.size(), CV_8UC3);
    using cpoints = std::vector<cv::Point>;
    std::vector<cpoints> contours;
    cv::findContours(morpho, contours, cv::RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

    auto it = std::remove_if(std::begin(contours), std::end(contours), [](cpoints const &lhs)
    {
            return cv::boundingRect(lhs).area() < 100000;
});
    contours.erase(it, std::end(contours));

    cv::RNG rng(12345);
    for(size_t i = 0; i != contours.size(); ++i){
        cv::Scalar const color = cv::Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
        cv::drawContours(drawing, contours, static_cast<int>(i), color, 2);
        cv::rectangle(input, cv::boundingRect(contours[i]), color, 2);
        cv::imshow("drawing", drawing);
        cv::waitKey();
    }

    cv::imshow("input", input);
    cv::imshow("blur", blur_img);
    cv::imshow("edges", edges);
    cv::imshow("morpho", morpho);
    cv::imwrite("marble_region.jpg", input);
    cv::imwrite("morho.jpg", morpho);
    cv::waitKey();
}

using namespace std;
using namespace cv;

std::tuple<vector<KeyPoint>, vector<KeyPoint>, vector<DMatch>>
find_good_points_to_draw(vector<KeyPoint> const &matches1,
                         vector<KeyPoint> const &matches2,
                         cv::Mat const &hmat)
{
    constexpr double inlier_threshold = 2.5;
    vector<KeyPoint> inliers1, inliers2;
    std::vector<cv::DMatch> good_matches;
    //measure distance of feature points after mapping
    //if their distance is small enough, we treat them
    //as inliers and vice versa
    for(size_t i = 0; i != matches1.size(); ++i){
        cv::Mat col = cv::Mat::ones(3,1, CV_64F);
        col.at<double>(0) = matches1[i].pt.x;
        col.at<double>(1) = matches1[i].pt.y;
        col = hmat * col;
        col /= col.at<double>(2);
        double const dist = std::sqrt(std::pow(col.at<double>(0) - matches2[i].pt.x, 2) +
                                      std::pow(col.at<double>(1) - matches2[i].pt.y, 2));
        if(dist < inlier_threshold){
            int const new_i = static_cast<int>(inliers1.size());
            inliers1.emplace_back(matches1[i]);
            inliers2.emplace_back(matches2[i]);
            good_matches.emplace_back(cv::DMatch(new_i, new_i, 0));
        }
    }

    return std::make_tuple(std::move(inliers1), std::move(inliers2), std::move(good_matches));
}

Mat find_homography(Mat const &train, Mat const &query)
{
    Ptr<AKAZE> akaze = AKAZE::create();
    vector<KeyPoint> query_kpts, train_kpts;
    cv::Mat query_desc, train_desc;
    akaze->detectAndCompute(train, cv::noArray(), query_kpts, query_desc);
    akaze->detectAndCompute(query, cv::noArray(), train_kpts, train_desc);

    BFMatcher matcher(NORM_HAMMING);
    vector<vector<DMatch>> nn_matches;
    //top 2 matches because we need to apply David Lowe's ratio test
    matcher.knnMatch(train_desc, query_desc, nn_matches, 2);

    vector<KeyPoint> matches1, matches2;
    for(auto const &m : nn_matches){
        float const dist1 = m[0].distance;
        float const dist2 = m[1].distance;
        if(dist1 < 0.7 * dist2){
            matches1.emplace_back(train_kpts[m[0].queryIdx]);
            matches2.emplace_back(query_kpts[m[0].trainIdx]);
        }
    }

    if(matches1.size() > 4){
        std::vector<cv::Point2f> points1, points2;
        for(size_t i = 0; i != matches1.size(); ++i){
            points1.emplace_back(matches1[i].pt);
            points2.emplace_back(matches2[i].pt);
        }
        Mat hmat = cv::findHomography(points1, points2, cv::RANSAC, 5.0);
        Mat match_img;
        vector<KeyPoint> inliers_1, inliers_2;
        vector<DMatch> good_matches;
        std::tie(inliers_1, inliers_2, good_matches) = find_good_points_to_draw(matches1, matches2, hmat);
        drawMatches(query, inliers_1, train, inliers_2, good_matches, match_img);
        cv::resize(match_img, match_img, {}, 0.5, 0.5);
        imwrite("match_img.jpg", match_img);

        return hmat;
    }

    return {};
}

pair<Mat, vector<Point2f>> get_target_marble(Mat const &input, vector<Point2f> const &src_pts)
{
    using namespace cv;
    using namespace std;

    Point2f const tl = src_pts[0];
    Point2f const tr = src_pts[1];
    Point2f const br = src_pts[2];
    Point2f const bl = src_pts[3];

    auto const euclidean_dist = [](Point const &a, Point const &b)
    {
        return std::sqrt(std::pow(a.x-b.x, 2) + std::pow(a.y - b.y, 2));
    };
    int const max_width = static_cast<int>(std::max(euclidean_dist(br, bl), euclidean_dist(tr, tl)));
    int const max_height = static_cast<int>(std::max(euclidean_dist(tr, br), euclidean_dist(tl, bl)));

    vector<Point2f> const src{tl, tr, br, bl};
    vector<Point2f> dst{Point(0,0), Point(max_width -1,0), Point(max_width-1,max_height-1), Point(0,max_height-1)};
    Mat const hmat = getPerspectiveTransform(src, dst);
    Mat target;
    warpPerspective(input, target, hmat, {max_width, max_height});

    return std::make_pair(std::move(target), std::move(dst));
}

}


/**
 * This file solve the problem at
 * https://stackoverflow.com/questions/46504836/find-contours-in-images-with-complex-background-and-rich-texture-in-opencv-3-3/46520615#46520615
 */

void marble_slab()
{
    using namespace cv;
    using namespace std;

    Mat input = imread("../forum_quest/data/complex_bg.jpg");
    if(input.empty()){
        cerr<<"cannot open image\n";
        return;
    }

    Point2f const tl = Point(50,18);
    Point2f const tr = Point(1230,78);
    Point2f const br = Point(1229,922);
    Point2f const bl = Point(51,884);
    vector<Point2f> const src{tl, tr, br, bl};
    vector<Point2f> dst;
    Mat target;
    std::tie(target, dst) = get_target_marble(input, src);
    Mat const hmat = find_homography(input, target);

    if(!hmat.empty()){

        vector<Point2f> query_points;
        vector<Point> qpts;
        perspectiveTransform(dst, query_points, hmat);
        for(auto const &pt : query_points){
            cout<<pt<<endl;
            qpts.emplace_back(pt);
        }

        polylines(input, qpts, true, {255,0,0}, 2);

        imshow("crop", target);
        imshow("result", input);
        waitKey();

        cv::resize(target, target, {}, 0.5, 0.5);
        cv::resize(input, input, {}, 0.5, 0.5);
        imwrite("crop.jpg", target);
        imwrite("marble_result.jpg", input);
    }else{
        cerr<<"cannot find homography matrix";
    }
}
