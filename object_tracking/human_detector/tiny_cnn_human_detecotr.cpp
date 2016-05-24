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

constexpr double human_aspect = 1.4;

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
    /*if(verbose_){
        for(auto const &val : presult){
            std::cout<<val.first<<", "<<val.second<<"\n";
        }
        std::cout<<std::endl;
    }//*/
    if(presult.size() == 2){        
        auto const max = presult[0].first > presult[1].first ?
                                           presult[0] : presult[1];

        return max.second == 1 && max.first >= threshold_;
    }

    return false;
}

std::vector<cv::Rect2d> tiny_cnn_human_detector::
search_simple(const cv::Mat &input)
{
    preprocess(input);

    std::vector<cv::Rect2d> results;
    cv::Size2i const min_size(20, 35);
    cv::Size2i const step(4,4);
    cv::Size2i const win_size(20, 35);
    double const scale = 1.5;
    ocv::saliency::pyramid_scan scanner(min_size, win_size,
                                        step, scale);

    auto in_copy = input.clone();
    auto detector = [&results, &in_copy, &input, this]
            (int row, int col, cv::Mat const &sub_img,
             double wratio, double hratio)
    {        
        /*if(is_human(sub_img, false)){
            results.emplace_back(col*wratio, row*hratio,
                                 sub_img.cols*wratio,
                                 sub_img.rows*hratio);
        }*/
        cv::rectangle(in_copy,
                      cv::Rect(col, row, sub_img.cols, sub_img.rows),
        {255,0,0});
        cv::imshow("sub image", in_copy);
        cv::waitKey();
        input.copyTo(in_copy);
    };

    scanner.scan(input, detector);

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
