#include "tiny_cnn_human_detecotr.hpp"

#include <ocv_libs/dlib/cv_to_dlib.hpp>
#include <ocv_libs/saliency/pyramid_scan.hpp>
#include <ocv_libs/tiny_cnn/image_converter.hpp>
#include <ocv_libs/tiny_cnn/predictor.hpp>

#include <dlib/image_processing.h>
#include <dlib/opencv.h>

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <fstream>

namespace{

constexpr double human_aspect = 2;

}

tiny_cnn_human_detector::
tiny_cnn_human_detector(size_t max_player) :
    max_player_(max_player),
    min_area_(700),
    predictor_(create_network()),
    threshold_(0.8),
    verbose_(false)
{

}

bool tiny_cnn_human_detector::is_human(const cv::Mat &input)
{
    return is_human(input, true);
}

void tiny_cnn_human_detector::preprocess(const cv::Mat &input)
{
    if(input.channels() == 3){
        cv::cvtColor(input, gray_img_, CV_BGR2GRAY);
    }else{
        input.copyTo(gray_img_);
    }
}

bool tiny_cnn_human_detector::is_human(const cv::Mat &input, bool do_preprocess)
{
    if(input.rows / static_cast<double>(input.cols) <
            human_aspect){
        return false;
    }

    if(do_preprocess){
        preprocess(input);
    }

    cv::resize(gray_img_, candidate_mat_, {64,64});
    ocv::tcnn::cvmat_to_img(candidate_mat_, candiate_img_);
    auto const presult = predictor_.predict(candiate_img_);
    if(verbose_){
        for(auto const &val : presult){
            std::cout<<val.first<<", "<<val.second<<"\n";
        }
        std::cout<<std::endl;
    }
    if(presult.size() == 2){
        auto const pair = std::minmax(std::begin(presult), std::end(presult));
        if(pair.first->second == 1 &&
                pair.first->first >= threshold_){
            if(verbose_){
                cv::imshow("player", input);
                cv::waitKey();
            }
            return true;
        }
    }

    return false;
}

std::vector<cv::Rect2d> tiny_cnn_human_detector::
search_simple(const cv::Mat &input)
{
    preprocess(input);

    dlib::cv_image<uchar> dimg(gray_img_);
    constexpr size_t candidate_width = 32;
    constexpr size_t candidate_height = candidate_width * human_aspect;
    constexpr size_t candidate_size = candidate_width*candidate_height;

    std::vector<dlib::rectangle> rects;
    dlib::find_candidate_object_locations
            (dimg, rects, dlib::linspace(50,200,3),
             candidate_size);
    dlib::remove_duplicates(rects);
    std::vector<cv::Rect2d> results =
            ocv::odlib::rect_to_cv_rect<double>(rects);

    auto not_human = [&](cv::Rect2d const &a)
    {
        return !is_human(gray_img_(a), false);
    };
    auto it = std::remove_if(std::begin(results),
                             std::end(results),
                             not_human);
    results.erase(it, std::end(results));

    return results;
}

std::vector<cv::Rect2d> tiny_cnn_human_detector::search(const cv::Mat &input)
{
    return search_simple(input);
}

void tiny_cnn_human_detector::set_min_area(double value)
{
    min_area_ = value;
}

void tiny_cnn_human_detector::set_threshold(double threshold)
{
    threshold_ = threshold;
}

void tiny_cnn_human_detector::set_verbose(bool value)
{
    verbose_ = value;
}

tiny_cnn_human_detector::NetType
tiny_cnn_human_detector::create_network()
{        
    using namespace tiny_cnn;
    using namespace tiny_cnn::activation;
    using activate = leaky_relu;

    NetType net;
    net << convolutional_layer<activate>(64, 64, 3, 1, 12, padding::same)
        << max_pooling_layer<activate>(64, 64, 12, 2)
        << convolutional_layer<activate>(32, 32, 3, 12, 18, padding::same)
        << max_pooling_layer<activate>(32, 32, 18, 2)
        << convolutional_layer<activate>(16, 16, 5, 18, 24)
        << fully_connected_layer<softmax>(12*12*24, 2);

    std::ifstream in("human_weights_create_arch_64_04_128_0.300000");
    if(!in.is_open()){
        throw std::runtime_error("cannot open tiny cnn weights file");
    }

    in>>net;

    return net;
}
