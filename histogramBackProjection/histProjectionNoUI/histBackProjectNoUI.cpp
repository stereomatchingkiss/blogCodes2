#include <array>
#include <iostream>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <basicImageAlgo.hpp> //for OCV::for_each_channels
#include <imageAlgorithm.hpp> //for OCV::calc_histogram and OCV::calc_back_project

#include "histBackProjectNoUI.hpp"

namespace{

inline cv::MatND get_hist(cv::Mat &input, int histogram_dimension)
{        
    if(histogram_dimension == 2){
        return OCV::calc_histogram<2>({input}, {0, 1}, {180, 256}, {{ {0, 180}, {0, 256} }});
    }else{
        return OCV::calc_histogram<1>({input}, {0}, {180}, {{ {0, 180} }});
    }
}

}

/**
 * @brief overload of custom_back_project
 */
std::pair<cv::Mat, cv::Mat> histBackProjectNoUI::custom_back_project(int histogram_dimension)
{
    return custom_back_project(histogram_dimension, cv::Size(11, 11), cv::MORPH_ELLIPSE);
}

/**
 * @brief implementation of back projection
 * @param histogram_dimension : the dimension of the histograms
 * @param filter_size : size of the noise removing(?) filter
 * @param morph_type : shape of the morphology filter
 * @return first : probability map; second : final results(separated target)
 */
std::pair<cv::Mat, cv::Mat> histBackProjectNoUI::custom_back_project(int histogram_dimension, cv::Size const &filter_size, int morph_type)
{
    generate_ratio(histogram_dimension);
    std::pair<cv::Mat, cv::Mat> results;
    results.first = generate_probability_map(histogram_dimension);
    remove_noise(probability_map_, filter_size, morph_type);
    results.first = probability_map_.clone();
    results.second = separate_target_and_background(probability_map_);

    return results;
}

std::pair<cv::Mat, cv::Mat> histBackProjectNoUI::custom_back_project(cv::Mat const &src, cv::Mat const &model, int histogram_dimension, const cv::Size &filter_size, int morph_type)
{
    set_input_and_model(src, model);
    return custom_back_project(histogram_dimension, filter_size, morph_type);
}

/**
 * @brief use the function cv::calcBackProject() to implement histogram backProjection
 * @param histogram_dimension : the dimension of the histograms
 * @param filter_size : size of the noise removing(?) filter
 * @param morph_type : shape of the morphology filter
 * @return first : probability map; second : final results(separated target)
 */
std::pair<cv::Mat, cv::Mat> histBackProjectNoUI::openCV_back_project(int histogram_dimension)
{
   return openCV_back_project(histogram_dimension, cv::Size(11, 11), cv::MORPH_ELLIPSE);
}

/**
 * @brief use the function cv::calcBackProject() to implement histogram backProjection
 * @param histogram_dimension : the dimension of the histograms
 * @param filter_size
 * @param morph_type
 * @return
 */
std::pair<cv::Mat, cv::Mat> histBackProjectNoUI::openCV_back_project(int histogram_dimension, cv::Size const &filter_size, int morph_type)
{
    auto model_hist = get_hist(model_hsv_, histogram_dimension);
    cv::Mat probability_map;
    if(histogram_dimension == 2){
        probability_map = OCV::calc_back_project<2>({input_hsv_}, {0, 1}, model_hist, {{ {0, 180}, {0, 256} }});
    }else{
        probability_map = OCV::calc_back_project<1>({input_hsv_}, {0}, model_hist, {{ {0, 180} }});
    }

    std::pair<cv::Mat, cv::Mat> results;
    results.first = probability_map.clone();
    remove_noise(probability_map, filter_size, morph_type);
    results.second = separate_target_and_background(probability_map);

    return results;
}

std::pair<cv::Mat, cv::Mat> histBackProjectNoUI::openCV_back_project(cv::Mat const &src, cv::Mat const &model, int histogram_dimension, cv::Size const &filter_size, int morph_type)
{
    set_input_and_model(src, model);
    return openCV_back_project(histogram_dimension, filter_size, morph_type);
}

void histBackProjectNoUI::run()
{
    model_ = cv::imread("/Users/Qt/program/blogsCodes/pic/rose_region.png");
    input_ = cv::imread("/Users/Qt/program/blogsCodes/pic/rose_small.png");

    if(input_.empty() || model_.empty()){
        std::cerr<<"input or model do not exist"<<std::endl;
        return;
    }

    try{
        input_hsv_ = convert_to_hsv(input_);
        model_hsv_ = convert_to_hsv(model_);

        auto custom_one_dim_results = custom_back_project(1);
        auto custom_two_dim_results = custom_back_project(2);
        auto opencv_one_dim_results = openCV_back_project(1);
        auto opencv_two_dim_results = openCV_back_project(2);

        cv::imwrite("map_of_1_dim_histogram(custom).png", custom_one_dim_results.first);
        cv::imwrite("result_of_1_dim_histogram(custom).png", custom_one_dim_results.second);
        cv::imwrite("map_of_2 dim_histogram(custom).png", custom_two_dim_results.first);
        cv::imwrite("result_of_2_dim_histogram(custom).png", custom_two_dim_results.second);

        cv::imwrite("map_of_1_dim_histogram(openCV).png", opencv_one_dim_results.first);
        cv::imwrite("result_of_1_dim histogram(openCV).png", opencv_one_dim_results.second);
        cv::imwrite("map_of_2_dim_histogram(openCV).png", opencv_two_dim_results.first);
        cv::imwrite("result_of_2_dim histogram(openCV).png", opencv_two_dim_results.second);
    }catch(std::exception const &ex){
        std::cerr<<ex.what()<<std::endl;
    }
}

void histBackProjectNoUI::set_input_and_model(cv::Mat const &src, cv::Mat const &model)
{
    input_ = src.clone();
    model_ = model.clone();
    input_hsv_ = convert_to_hsv(src);
    model_hsv_ = convert_to_hsv(model);
}

/**********************************************************
 ****************** implementation ************************
 **********************************************************/

cv::Mat histBackProjectNoUI::convert_to_hsv(cv::Mat const &input) const
{
    cv::Mat hsv = input.clone();
    if(hsv.channels() == 4){
        cv::cvtColor(hsv, hsv, CV_BGRA2BGR);
    }

    cv::cvtColor(hsv, hsv, CV_BGR2HSV);

    return hsv;
}

cv::Mat histBackProjectNoUI::generate_probability_map(int histogram_dimension)
{               
    if(histogram_dimension == 2){
        std::vector<cv::Mat> hsv;
        cv::split(input_hsv_, hsv);
        probability_map_.create(hsv[0].size(), CV_32F);
        auto map_ptr = probability_map_.ptr<float>(0);
        OCV::for_each_channels<uchar>(hsv[0], hsv[1], [&](int hue, int saturation)
        {
            *map_ptr = std::min<float>(*(ratio_.ptr<float>(hue) + saturation), 1.0);
            ++map_ptr;
        });
    }else{
        cv::Mat hue(input_hsv_.size(), CV_8U);
        OCV::mix_channels(input_hsv_, hue, {0, 0});
        hue.convertTo(hue, CV_32F);
        probability_map_.create(hue.size(), CV_32F);
        auto ratio_ptr = ratio_.ptr<float>(0);
        OCV::for_each_channels<float>(hue, probability_map_, [&](int hue_pix, float &probability_map_pix)
        {
            probability_map_pix = std::min<float>(*(ratio_ptr + hue_pix), 1.0);
        });
    }

    return probability_map_.clone();
}

void histBackProjectNoUI::generate_ratio(int histogram_dimension)
{    
    auto input_hist = get_hist(input_hsv_, histogram_dimension);
    auto model_hist = get_hist(model_hsv_, histogram_dimension);
    ratio_ = model_hist / (input_hist + 1);
}

void histBackProjectNoUI::remove_noise(cv::Mat &probability_map, cv::Size const &filter_size, int morph_type)
{
    cv::Mat disc = cv::getStructuringElement(morph_type, filter_size);
    cv::filter2D(probability_map, probability_map, -1, disc);
    cv::normalize(probability_map, probability_map, 0, 255, CV_MINMAX);
    probability_map.convertTo(probability_map, CV_8U);
}

cv::Mat histBackProjectNoUI::separate_target_and_background(cv::Mat &probability_map)
{
    cv::threshold(probability_map, mask_, 50, 255, cv::THRESH_BINARY);
    cv::Mat result;
    input_.copyTo(result, mask_);

    return result;
}
