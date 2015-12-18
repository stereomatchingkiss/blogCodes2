#include "morphology_localizer.hpp"

#include <ocv_libs/core/attribute.hpp>
#include <ocv_libs/core/resize.hpp>

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <iostream>

namespace{

constexpr double epsillon = 0.035;

class is_non_valid_contours
{
public:
    bool operator()(std::vector<cv::Point> const &contour)
    {
        auto const attribute =
                analyzer_.analyze(contour, epsillon);
        if(attribute.contour_area_ < 1200){
            return true;
        }

        if(attribute.bounding_rect_.width < 60 ||
                attribute.bounding_rect_.height < 20){
            return true;
        }

        if(attribute.aspect_ratio_ < 2.5 ||
                attribute.aspect_ratio_ > 7.54){
            return true;
        }

        return false;
    }

private:
    ocv::contour_analyzer analyzer_;
};

}

morphology_localizer::morphology_localizer() :
    //make the ratio of the tophat_kernel_ close to license plate
    blackhat_kernel_(cv::getStructuringElement(cv::MORPH_RECT, blackhat_kernal_size_))
{    
}

std::vector<std::vector<cv::Point> > const&
morphology_localizer::get_contours() const
{
    return contours_;
}

cv::Mat& morphology_localizer::get_gray_input()
{
    return gray_input_;
}

cv::Mat const& morphology_localizer::get_gray_input() const
{
    return gray_input_;
}

void morphology_localizer::
localize_plate(const cv::Mat &input)
{
    preprocess(input);
    find_plate_contours();
}

void morphology_localizer::set_show_debug_message(bool value)
{
    debug_ = value;
}

void morphology_localizer::set_blackhat_size(const cv::Size &value)
{
    blackhat_kernal_size_ = value;
}

void morphology_localizer::find_plate_contours()
{
    cv::findContours(binary_input_, contours_,
                     CV_RETR_EXTERNAL,
                     CV_CHAIN_APPROX_SIMPLE);

    //erase non number plate contour
    auto it = std::remove_if(std::begin(contours_),
                             std::end(contours_),
                             is_non_valid_contours());
    contours_.erase(it, std::end(contours_));

    if(debug_){
        ocv::print_contour_attribute_name(std::cout);
        cv::Mat input_copy;
        cv::imshow("resize input", resize_input_);
        for(auto const &contour : contours_){
            constexpr int Thickness = 2;
            resize_input_.copyTo(input_copy);
            cv::rectangle(input_copy, cv::boundingRect(contour),
            {255,0,0}, Thickness);
            ocv::print_contour_attribute(contour, epsillon, std::cout);
            cv::imshow("contour", input_copy);
            cv::imshow("region", resize_input_(cv::boundingRect(contour)));
            cv::waitKey();
        }
        cv::waitKey();
    }
}

void morphology_localizer::create_light_input()
{
    auto const rect_kernel =
            cv::getStructuringElement(cv::MORPH_RECT, {3,3});
    cv::morphologyEx(gray_input_, light_input_, cv::MORPH_CLOSE,
                     rect_kernel);
    cv::threshold(light_input_, light_input_, 50, 255, cv::THRESH_BINARY);
    //show_img_for_debug("light input", light_input_);
}

void morphology_localizer::reveal_dark_area()
{
    //use blackhat operations to reveal black area from white background
    //in another words, make the text on license plate more obvious
    cv::morphologyEx(gray_input_, morphology_input_, cv::MORPH_BLACKHAT,
                     blackhat_kernel_);
    show_img_for_debug("0 : black hat", morphology_input_);
}

void morphology_localizer::show_img_for_debug(const std::string &name,
                                              const cv::Mat &input) const
{
    if(debug_){
        cv::imshow(name, input);
    }
}

void morphology_localizer::binarize_image()
{
    //make the text of the license plate more obvious
    cv::Sobel(morphology_input_, gradient_input_, CV_8U, 1, 0, 3);
    //show_img_for_debug("1 : gradient", gradient_input_);

    //remove noise
    cv::GaussianBlur(gradient_input_, blur_input_, {7,7}, 0);
    //show_img_for_debug("2 : blur", blur_input_);

    //fill the gap between text
    cv::morphologyEx(blur_input_, morphology_input_, cv::MORPH_CLOSE,
                     blackhat_kernel_);
    //show_img_for_debug("3 : close", morphology_input_);
    cv::threshold(morphology_input_, binary_input_, 0, 255,
                  CV_THRESH_BINARY | CV_THRESH_OTSU);
    //show_img_for_debug("4 : binary", binary_input_);
}

void morphology_localizer::remove_noise()
{
    auto const rect_kernel =
            cv::getStructuringElement(cv::MORPH_RECT, {3,3});
    cv::erode(binary_input_, morphology_input_, rect_kernel, {-1,-1}, 2);
    cv::dilate(morphology_input_, binary_input_, rect_kernel, {-1,-1}, 2);
    //show_img_for_debug("5 : remove noise", binary_input_);

    //the result of last step look almost the same as step 5 in many cases,
    //but it some of the cases, it could introduce big difference
    create_light_input();
    cv::bitwise_and(binary_input_, light_input_, binary_input_);
    cv::dilate(binary_input_, morphology_input_, rect_kernel, {-1,-1}, 2);
    cv::erode(morphology_input_, binary_input_, rect_kernel, {-1,-1}, 2);
    //show_img_for_debug("6 : remove more noise", binary_input_);
}

void morphology_localizer::preprocess(const cv::Mat &input)
{    
    CV_Assert(input.type() == CV_8UC3);

    ocv::resize_aspect_ratio(input, resize_input_, {640, 0});
    cv::cvtColor(resize_input_, gray_input_, CV_BGR2GRAY);

    //show_img_for_debug("resize origin", resize_input_);
    reveal_dark_area();
    binarize_image();
    remove_noise();
}
