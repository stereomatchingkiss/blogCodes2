#include "features_utility.hpp"

#include <opencv2/calib3d.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/xfeatures2d.hpp>

#include <iostream>
#include <vector>

using namespace cv;
using namespace std;

features_utility::features_utility()
{

}

std::tuple<vector<KeyPoint>, vector<KeyPoint>>
features_utility::get_match_points(const Mat &train, const Mat &query)
{
    Ptr<Feature2D> akaze = AKAZE::create();
    vector<KeyPoint> query_kpts, train_kpts;
    cv::Mat query_desc, train_desc;
    Mat gray;
    cvtColor(train, gray, CV_BGR2GRAY);
    akaze->detectAndCompute(gray, cv::noArray(), query_kpts, query_desc);
    cvtColor(query, gray, CV_BGR2GRAY);
    akaze->detectAndCompute(gray, cv::noArray(), train_kpts, train_desc);

    BFMatcher matcher(NORM_HAMMING);
    vector<vector<DMatch>> nn_matches;
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

    return std::make_tuple(std::move(matches1), std::move(matches2));
}

Mat features_utility::draw_match_points(const cv::Mat &train,
                                        const cv::Mat &query,
                                        const features_utility::key_point_vec &matches1,
                                        const features_utility::key_point_vec &matches2,
                                        const Mat &homography)
{
    constexpr double inlier_threshold = 2.5;
    Mat match_img;
    vector<KeyPoint> inliers1, inliers2;
    std::vector<cv::DMatch> good_matches;
    //measure distance of feature points after mapping
    //if their distance is small enough, we treat them
    //as inliers and vice versa
    for(size_t i = 0; i != matches1.size(); ++i){
        cv::Mat col = cv::Mat::ones(3,1, CV_64F);
        col.at<double>(0) = matches1[i].pt.x;
        col.at<double>(1) = matches1[i].pt.y;
        col = homography * col;
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


    drawMatches(query, inliers1, train, inliers2, good_matches, match_img);

    return match_img;
}

Mat features_utility::draw_match_points(const Mat &train, const Mat &query)
{
    vector<KeyPoint> matches1, matches2;
    cv::Mat hmat;
    std::tie(matches1, matches2, hmat) = find_homography(train, query);
    std::cout<<"match points:"<<matches1.size()<<","<<matches2.size()<<std::endl;

    if(!hmat.empty()){
        return draw_match_points(train, query, matches1, matches2, hmat);
    }

    return {};
}

std::tuple<vector<KeyPoint>, vector<KeyPoint>, cv::Mat>
features_utility::find_homography(const Mat &train, const Mat &query)
{    
    vector<KeyPoint> matches1, matches2;
    std::tie(matches1, matches2) = get_match_points(train, query);

    if(matches1.size() > 4){
        std::vector<cv::Point2f> points1, points2;
        for(size_t i = 0; i != matches1.size(); ++i){
            points1.emplace_back(matches1[i].pt);
            points2.emplace_back(matches2[i].pt);
        }
        Mat hmat = cv::findHomography(points1, points2, cv::RANSAC, 5.0);

        return std::make_tuple(std::move(matches1), std::move(matches2), std::move(hmat));
    }//*/

    return {};
}
