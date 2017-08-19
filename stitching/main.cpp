#include "feature_stitch.hpp"

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/stitching.hpp>

#include <functional>
#include <iostream>

using namespace std;

cv::Mat read_img(std::string const &img_location);

void test_feature_solution(std::string const &folder,
                           std::string const &img1_name,
                           std::string const &img2_name,
                           std::function<std::pair<cv::Mat, cv::Mat>(cv::Mat const&, cv::Mat const&)> stitcher);

int main()try
{    
    std::string const folder("C:/Users/yyyy/Qt/computer_vision_dataset/stitching/images/");
    std::string const img1_location("bryce_left_02.png");
    std::string const img2_location("bryce_right_02.png");
    test_feature_solution(folder, img1_location, img2_location, [](cv::Mat const &img1, cv::Mat const &img2)
    {
        feature_stitch fs;
        return fs.stitch_images(img1, img2, stitch_direction::horizontal, 0.75, 4.0, true);
    });//*/

    /*test_feature_solution(folder, img1_location, img2_location, [](cv::Mat const &img1, cv::Mat const &img2)
    {
       cv::Ptr<cv::Stitcher> stitcher = cv::Stitcher::create(cv::Stitcher::SCANS, false);
       std::vector<cv::Mat> imgs{img1, img2};
       cv::Mat stitch_result;
       cv::Stitcher::Status const status = stitcher->stitch(imgs, stitch_result);
       if(status != cv::Stitcher::OK){
           cerr<<"can't stich images by cv::Stitcher, error code = "<<int(status) << endl;
           throw std::runtime_error("can't stich images by cv::Stitcher, error code = " +
                                    std::to_string(static_cast<int>(status)));
       }

       return std::make_pair(stitch_result, cv::Mat());
    });//*/

    return 0;
}catch(std::exception const &ex){
    cerr<<ex.what()<<endl;
}

cv::Mat read_img(std::string const &img_location)
{
    cv::Mat img = cv::imread(img_location);
    if(img.empty()){
        throw std::runtime_error("cannot read img:" + img_location);
    }

    return img;
}

void test_feature_solution(std::string const &folder,
                           std::string const &img1_name,
                           std::string const &img2_name,
                           std::function<std::pair<cv::Mat, cv::Mat>(cv::Mat const&, cv::Mat const&)> stitcher)
{
    auto img1 = read_img(folder + img1_name);
    auto img2 = read_img(folder + img2_name);

    std::cout<<img2.size()<<","<<img1.size()<<std::endl;
    cv::resize(img1, img1, cv::Size(400, static_cast<int>(img1.rows * 400.0/img1.cols)));
    cv::resize(img2, img2, cv::Size(400, static_cast<int>(img2.rows * 400.0/img2.cols)));
    cv::resize(img2, img2, cv::Size(img2.cols, img1.rows));

    cv::Mat stitch_result;
    cv::Mat matches_points;
    std::tie(stitch_result, matches_points) = stitcher(img1, img2);

    if(!stitch_result.empty()){
        cv::imshow("stitch_result", stitch_result);
        cv::imwrite("stitch_resut.jpg", stitch_result);
    }
    if(!matches_points.empty()){
        cv::imshow("matches_points", matches_points);
        cv::imwrite("matches_points.jpg", matches_points);
    }
    if(!stitch_result.empty() || !matches_points.empty()){
        cv::imshow("img1", img1);
        cv::imshow("img2", img2);
        cv::waitKey();
    }
}

