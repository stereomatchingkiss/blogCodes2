#include "segment_character.hpp"
#include "utility.hpp"

#include <ocv_libs/core/attribute.hpp>
#include <ocv_libs/core/for_each.hpp>
#include <ocv_libs/core/perspective_transform.hpp>
#include <ocv_libs/core/resize.hpp>

#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

segment_character::segment_character(size_t min_char_width,
                                     size_t min_char_num,
                                     size_t max_char_num) :
    min_char_width_{min_char_width},
    min_char_num_{min_char_num},
    max_char_num_{max_char_num}
{

}

bool segment_character::
detect_characters(const cv::Mat &input,
                  const segment_character::contour_type &contour)
{
    generate_bird_eyes_view(input, contour);
    //plate_ = input(cv::boundingRect(contour));
    binarize_plate();
    split_character();
    cv::destroyAllWindows();

    return chars_contour_.size() >= min_char_num_ ? true : false;
}

const cv::Mat &segment_character::get_binary_plate() const
{
    return binary_plate_;
}

const cv::Mat &segment_character::get_bird_eyes_plate() const
{
    return bird_eyes_plate_;
}

const segment_character::contours_type &segment_character::
get_chars_contours() const
{
    return chars_contour_;
}

segment_character::contours_type &segment_character::get_chars_contours()
{
    return chars_contour_;
}

void segment_character::set_img_name(const std::string &value)
{
    img_name_ = value;
}

void segment_character::set_min_char_width(size_t value)
{
    min_char_width_ = value;
}

void segment_character::set_min_char_num(size_t value)
{
    min_char_num_ = value;
}

void segment_character::set_show_debug_message(bool value)
{
    debug_ = value;
}

void segment_character::binarize_plate()
{    
    binarize_image(bird_eyes_plate_, binary_plate_);
}

void segment_character::
bird_eyes_view_debug_message(cv::Mat const &input,
                             contour_type const &contour,
                             cv::Point2f const (&points)[4])
{
    if(debug_){
        auto input_cpy = input.clone();
        cv::Point2f sort_points[4];
        ocv::sort_corners(points, std::begin(sort_points));
        for(size_t i = 0; i != 4; ++i){
            //std::cout<<pt<<", x="<<pt.x<<", y="<<pt.y<<std::endl;
            std::cout<<i<<" : "<<sort_points[i]<<std::endl;
            auto text_point = sort_points[i];
            text_point.y -= 10;
            double const scale = 1.0;
            int const thickness = 2;
            cv::putText(input_cpy, std::to_string(i), text_point,
                        cv::FONT_HERSHEY_COMPLEX, scale, {255,0,0}, thickness);
            int const radius = 3;
            cv::circle(input_cpy, sort_points[i], radius,
            {0, 255, 0}, thickness);
        }
        std::cout<<"origin size : "<<input(cv::boundingRect(contour)).size()<<std::endl;
        std::cout<<"biry eyes size : "<<bird_eyes_plate_.size()<<std::endl;

        cv::imshow("origin plate", input(cv::boundingRect(contour)));
        cv::imshow("rotated point", input_cpy);
        cv::imshow("bird eyes plate", bird_eyes_plate_);
        ocv::resize_aspect_ratio(input_cpy, input_cpy, {320, 0});
        cv::imwrite("plate_points.jpg", input_cpy);
        cv::imwrite("origin_plate.jpg", input(cv::boundingRect(contour)));
        cv::imwrite("bird_eyes_plate.jpg", bird_eyes_plate_);
        cv::waitKey();
    }
}

void segment_character::
generate_bird_eyes_view(const cv::Mat &input,
                        const segment_character::contour_type &contour)
{        
    cv::Point2f pr_points[4];
    cv::minAreaRect(contour).points(pr_points);
    ocv::four_points_transform(input, bird_eyes_plate_, pr_points);

    //bird_eyes_view_debug_message(input, contour, pr_points);
}

void segment_character::generate_components()
{
    cv::Mat labels;
    int const nlabels =
            cv::connectedComponents(binary_plate_, labels);

    chars_mask_.resize(nlabels - 1);
    for(auto &mask : chars_mask_){
        mask.create(binary_plate_.size(), CV_8U);
        mask = 0;
    }

    for(int r = 0; r != labels.rows; ++r){
        auto labels_ptr = labels.ptr<int>(r);
        for(int c = 0; c != labels.cols; ++c){
            if(labels_ptr[c] != 0){
                chars_mask_[labels_ptr[c]-1].at<uchar>(r,c) = 255;
            }
        }
    }
}

bool segment_character::is_character_candidate(contour_type const &contour) const
{
    double const contour_area = cv::contourArea(contour);
    if(contour_area < 40){
        return false;
    }

    auto const bounding_rect = cv::boundingRect(contour);
    //std::cout<<"char width "<<bounding_rect.width<<std::endl;
    //if(bounding_rect.width < min_char_width_){
    //    return false;
    //}

    double const ratio = bounding_rect.width /
            static_cast<double>(bounding_rect.height);
    if(ratio > 1.0){
        return false;
    }

    double const extend = contour_area /
            static_cast<double>(bounding_rect.area());

    if(extend < 0.2){
        return false;
    }

    return true;
}

void segment_character::show_chars_contour()
{
    if(debug_){
        cv::Mat temp = bird_eyes_plate_.clone();
        for(int i = 0; i != chars_contour_.size(); ++i){
            cv::Point2f points[4];
            cv::minAreaRect(chars_contour_[i]).points(points);
            cv::Mat temp_char_trans = ocv::four_points_transform(bird_eyes_plate_, points);
            ocv::resize_aspect_ratio(temp_char_trans, temp_char_trans, {40, 0});
            //cv::imshow("temp_char_transform", temp_char_trans);
            cv::Mat temp_char = temp(cv::boundingRect(chars_contour_[i]));
            ocv::resize_aspect_ratio(temp_char, temp_char, {40, 0});
            //cv::imshow("temp_char", temp_char);
            cv::imwrite("char_" + std::to_string(i) + ".jpg", temp_char_trans);
            //cv::waitKey();
            //cv::destroyWindow("temp_char");
            //cv::destroyWindow("temp_char_transform");
            cv::drawContours(temp, chars_contour_, i, {0,255,0}, 2);
        }
        //cv::imshow("chars contours", temp);
        if(chars_contour_.size() >= min_char_num_ &&
                chars_contour_.size() < max_char_num_){
            cv::imwrite(img_name_+"_segment_contours.jpg", temp);
        }
        //cv::waitKey();
    }
}

void segment_character::show_chars_component(int j, size_t i,
                                             contours_type contours)
{
    if(debug_){
        cv::Mat dst(bird_eyes_plate_.size(), CV_8U);
        dst = 0;
        cv::drawContours(dst, contours, j, {255}, -1);
        auto const num = std::to_string(i) + "_" + std::to_string(j);
        if(is_character_candidate(contours[j])){
            std::cout<<("mask_" + num)<<" is char"<<std::endl;
        }else{
            std::cout<<("mask_" + num)<<" is not a char"<<std::endl;
        }
        ocv::print_contour_attribute(contours[j], 0.02, std::cout);
        cv::imshow("mask_" + num, dst);
        cv::waitKey();
        cv::destroyWindow("mask_" + num);
    }
}

void segment_character::split_character()
{    
    if(debug_){
        ocv::print_contour_attribute_name(std::cout);
    }

    generate_components();
    chars_contour_.clear();
    for(size_t i = 0; i != chars_mask_.size(); ++i){
        contours_type contours;
        cv::findContours(chars_mask_[i], contours,
                         cv::RETR_EXTERNAL,
                         cv::CHAIN_APPROX_SIMPLE);
        for(int j = 0; j != contours.size(); ++j){
            show_chars_component(j, i, contours);
            if(is_character_candidate(contours[j])){
                chars_contour_.emplace_back(std::move(contours[j]));
            }
        }
    }
    show_chars_contour();
}
