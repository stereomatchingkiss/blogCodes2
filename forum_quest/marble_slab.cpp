#include "marble_slab.hpp"

#include "features_utility.hpp"

#include <opencv2/calib3d.hpp>
#include <opencv2/core.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <iostream>

/**
 * This file solve the problem at
 * https://stackoverflow.com/questions/46504836/find-contours-in-images-with-complex-background-and-rich-texture-in-opencv-3-3/46520615#46520615
 *
 * lzg@jimstudio.net
 */

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

Mat read_image(std::string const &img_name)
{
    Mat mat = imread(img_name);
    if(mat.empty()){
        throw std::runtime_error("cannot read img:" + img_name);
    }

    return mat;
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

vector<Point2f> get_marble_points(std::string const &name, float ratio = 1)
{
    if(name == "marble_00_00"){
        Point2f const tl = Point2f(50,18) * ratio;
        Point2f const tr = Point2f(1230,78) * ratio;
        Point2f const br = Point2f(1229,922) * ratio;
        Point2f const bl = Point2f(51,884) * ratio;
        return {tl, tr, br, bl};
    }else if(name == "marble_01_00"){
        Point2f const tl = Point2f(36,65) * ratio;
        Point2f const tr = Point2f(990,69) * ratio;
        Point2f const br = Point2f(979,523) * ratio;
        Point2f const bl = Point2f(78,500) * ratio;
        return {tl, tr, br, bl};
    }else if(name == "marble_02_00"){
        Point2f const tl = Point2f(46,80) * ratio;
        Point2f const tr = Point2f(963,76) * ratio;
        Point2f const br = Point2f(955,577) * ratio;
        Point2f const bl = Point2f(65,574) * ratio;
        return {tl, tr, br, bl};
    }else if(name == "marble_03_00"){
        Point2f const tl = Point2f(76,85) * ratio;
        Point2f const tr = Point2f(1879,81) * ratio;
        Point2f const br = Point2f(1852,945) * ratio;
        Point2f const bl = Point2f(76,995) * ratio;
        return {tl, tr, br, bl};
    }else if(name == "marble_04_00"){
        Point2f const tl = Point2f(36,173) * ratio;
        Point2f const tr = Point2f(1249,167) * ratio;
        Point2f const br = Point2f(1225,768) * ratio;
        Point2f const bl = Point2f(14,826) * ratio;
        return {tl, tr, br, bl};
    }else if(name == "marble_05_00"){
        Point2f const tl = Point2f(40,71) * ratio;
        Point2f const tr = Point2f(1216,81) * ratio;
        Point2f const br = Point2f(1223,847) * ratio;
        Point2f const bl = Point2f(49,896) * ratio;
        return {tl, tr, br, bl};
    }else if(name == "marble_06_00"){
        Point2f const tl = Point2f(60,11) * ratio;
        Point2f const tr = Point2f(741,19) * ratio;
        Point2f const br = Point2f(732,451) * ratio;
        Point2f const bl = Point2f(44,445) * ratio;
        return {tl, tr, br, bl};
    }else if(name == "marble_07_00"){
        Point2f const tl = Point2f(16,78) * ratio;
        Point2f const tr = Point2f(929,81) * ratio;
        Point2f const br = Point2f(930,577) * ratio;
        Point2f const bl = Point2f(40,584) * ratio;
        return {tl, tr, br, bl};
    }else if(name == "marble_08_00"){
        Point2f const tl = Point2f(55,33) * ratio;
        Point2f const tr = Point2f(744,43) * ratio;
        Point2f const br = Point2f(728,439) * ratio;
        Point2f const bl = Point2f(58,419) * ratio;
        return {tl, tr, br, bl};
    }else if(name == "marble_09_00"){
        Point2f const tl = Point2f(109,369) * ratio;
        Point2f const tr = Point2f(3508,399) * ratio;
        Point2f const br = Point2f(3340,2193) * ratio;
        Point2f const bl = Point2f(117,2180) * ratio;
        return {tl, tr, br, bl};
    }

    return {};
}

Mat find_hs_hist(Mat const &input)
{
    int hist_size[] = {180, 256};
    int channels[] = {0, 1};

    float hranges[] = {0, 180};
    float sranges[] = {0, 256};

    const float* ranges[] = {hranges, sranges};

    Mat input_hist;
    calcHist(&input, 1, channels, Mat(), input_hist, 2, hist_size, ranges);

    return input_hist;
}

void histogram_backprojection()
{
    std::string const marble_name("marble_02_00");
    Mat input = read_image("../forum_quest/data/" + marble_name + ".jpg");
    //Mat target = read_image("../forum_quest/data/" + marble_name + "_crop.jpg");
    Mat target = input(Rect(input.cols/2, input.rows/2, 200, 200)).clone();

    Mat input_hsv;
    Mat target_hsv;
    cvtColor(input, input_hsv, CV_BGR2HSV);
    cvtColor(target, target_hsv, CV_BGR2HSV);

    Mat const target_hist = find_hs_hist(target_hsv);
    int channels[] = {0, 1};
    float hranges[] = {0, 180};
    float sranges[] = {0, 256};
    const float* ranges[] = {hranges, sranges};
    Mat back_project;
    calcBackProject(&input_hsv, 1, channels, target_hist, back_project, ranges);
    /*cv::Mat const disc = cv::getStructuringElement(cv::MORPH_ELLIPSE,
                   cv::Size(9, 9));
    erode(back_project, back_project, disc);
    dilate(back_project, back_project, disc);//*/
    //cv::filter2D(back_project, back_project, -1, disc);
    cv::normalize(back_project, back_project, 0, 255, CV_MINMAX);
    back_project.convertTo(back_project, CV_8U);

    cv::Mat mask;
    cv::threshold(back_project, mask, 200, 255, cv::THRESH_BINARY);
    cv::Mat result;
    input.copyTo(result, mask);

    imshow("back project", back_project);
    imshow("result", result);
    waitKey();
}

void feature_matches()
{
    std::string const marble_name("marble_00_00");
    Mat input = imread("../forum_quest/data/" + marble_name + ".jpg");
    if(input.empty()){
        cerr<<"cannot open image\n";
        return;
    }

    vector<Point2f> marble_crop_points;
    Mat target;
    std::tie(target, marble_crop_points) = get_target_marble(input, get_marble_points(marble_name));
    imwrite(marble_name + "_crop.jpg", target);
    //target = imread("../forum_quest/data/" + marble_name + "_crop.jpg");
    /*target = imread("../forum_quest/data/marble_00_00_crop.jpg");
    if(target.empty()){
        cerr<<"target is empty\n";
    }
    marble_crop_points = vector<Point2f>{Point(0,0), Point(target.cols -1,0),
            Point(target.cols-1,target.rows-1), Point(0,target.rows-1)};//*/

    input = read_image("../forum_quest/data/marble_00_02.jpg");
    features_utility fu;
    features_utility::key_point_vec matches1, matches2;
    Mat hmat;
    std::tie(matches1, matches2, hmat) = fu.find_homography(input, target);

    if(!hmat.empty()){

        Mat match_draw = fu.draw_match_points(input, target, matches1, matches2, hmat);
        cv::resize(match_draw, match_draw, {}, 0.5, 0.5);
        vector<Point2f> query_points;
        vector<Point> qpts;
        perspectiveTransform(marble_crop_points, query_points, hmat);
        for(auto const &pt : query_points){
            cout<<pt<<endl;
            qpts.emplace_back(pt);
        }

        rectangle(input, boundingRect(qpts), {255, 0, 0}, 2);
        //polylines(input, qpts, true, {0,255,0}, 2);

        imshow("crop", target);
        imshow("draw", match_draw);
        imshow("result", input);
        waitKey();

        cv::resize(input, input, {}, 0.5, 0.5);
        imwrite("marble_result.jpg", input);
    }else{
        cerr<<"cannot find homography matrix";
    }
}

}


void marble_slab()
{
    //feature_matches();
    //histogram_backprojection();
    //threshold_segment();    
}
