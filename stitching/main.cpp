#include "feature_stitch.hpp"

#include <opencv2/core.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/stitching.hpp>

#include <chrono>
#include <functional>
#include <iostream>

using namespace std;

void measure_elapsed_time(std::function<void()> func);

cv::Mat read_img(std::string const &img_location);

void test_stitching(std::string const &folder,
                    std::string const &img1_name,
                    std::string const &img2_name,
                    std::function<std::pair<cv::Mat, cv::Mat>(cv::Mat const&, cv::Mat const&)> stitcher);

int main(int argc, char *argv[])try
{            
    if(argc < 5){
        cerr<<"Argument must not less than 4. They are folder of images, \n"
           <<"name of left image, name of right image, and stitch mode. Mode "
             "0 will use feature stitch, 1 will use Stitcher class of opencv.\n"
             "Example:\n"
             "stitching ../../computer_vision_dataset/stitching/images/ bryce_left_02.png bryce_right_02.png 0";

        return -1;
    }

    std::string const folder(argv[1]);
    std::string const img1_location(argv[2]);
    std::string const img2_location(argv[3]);

    int const mode = std::stoi(argv[4]);
    if(mode == 0){
        measure_elapsed_time([&]()
        {
            test_stitching(folder, img1_location, img2_location, [](cv::Mat const &img1, cv::Mat const &img2)
            {
                feature_stitch fs;
                return fs.stitch_images(img1, img2, stitch_direction::horizontal, 0.75, 4.0, false);
            });
        });
    }else{
        measure_elapsed_time([&]()
        {
            test_stitching(folder, img1_location, img2_location, [](cv::Mat const &img1, cv::Mat const &img2)
            {
                cv::Ptr<cv::Stitcher> stitcher = cv::Stitcher::create(cv::Stitcher::PANORAMA, true);
                std::vector<cv::Mat> imgs{img1, img2};
                cv::Mat stitch_result;
                cv::Stitcher::Status const status = stitcher->stitch(imgs, stitch_result);
                if(status != cv::Stitcher::OK){
                    cerr<<"can't stich images by cv::Stitcher, error code = "<<int(status) << endl;
                    throw std::runtime_error("can't stich images by cv::Stitcher, error code = " +
                                             std::to_string(static_cast<int>(status)));
                }

                return std::make_pair(stitch_result, cv::Mat());
            });
        });
    }

    return 0;
}catch(std::exception const &ex){
    cerr<<ex.what()<<endl;
}

void measure_elapsed_time(std::function<void()> func)
{
    std::chrono::system_clock::time_point start = std::chrono::system_clock::now();
    func();
    std::chrono::system_clock::time_point end = std::chrono::system_clock::now();
    std::cout<<std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()<<"ms"<<std::endl;
}

cv::Mat read_img(std::string const &img_location)
{
    cv::Mat img = cv::imread(img_location);
    if(img.empty()){
        throw std::runtime_error("cannot read img:" + img_location);
    }

    return img;
}

void test_stitching(std::string const &folder,
                    std::string const &img1_name,
                    std::string const &img2_name,
                    std::function<std::pair<cv::Mat, cv::Mat>(cv::Mat const&, cv::Mat const&)> stitcher)
{
    auto img1 = read_img(folder + img1_name);
    auto img2 = read_img(folder + img2_name);

    std::cout<<img2.size()<<","<<img1.size()<<std::endl;
    cv::resize(img1, img1, cv::Size(400, static_cast<int>(img1.rows * 400.0/img1.cols)));
    cv::resize(img2, img2, cv::Size(400, static_cast<int>(img1.rows)));

    size_t const pos1 = img1_name.find_last_of(".");
    size_t const pos2 = img2_name.find_last_of(".");
    cv::imwrite(img1_name.substr(0, pos1) + ".jpg", img1);
    cv::imwrite(img2_name.substr(0, pos2) + ".jpg", img2);

    cv::Mat stitch_result;
    cv::Mat matches_points;
    std::tie(stitch_result, matches_points) = stitcher(img1, img2);

    if(!stitch_result.empty()){
        //cv::imshow("stitch_result", stitch_result);
        cv::imwrite(img1_name.substr(0, pos1) + "_stitch_resut.jpg", stitch_result);
    }
    if(!matches_points.empty()){
        //cv::imshow("matches_points", matches_points);
        cv::imwrite("matches_points.jpg", matches_points);
    }
    if(!stitch_result.empty() || !matches_points.empty()){
        //cv::imshow("img1", img1);
        //cv::imshow("img2", img2);
        cv::waitKey();
    }
}

