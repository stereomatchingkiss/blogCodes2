#include "segment_character.hpp"

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
                  const segment_character::contour_type &contours)
{
    generate_bird_eyes_view(input, contours);
    binarize_plate();
    //split_character();

    return false;
}

void segment_character::set_min_char_width(size_t value)
{
    min_char_width_ = value;
}

void segment_character::set_min_char_num(size_t value)
{
    min_char_num_ = value;
}

void segment_character::set_max_char_num(size_t value)
{
    max_char_num_ = value;
}

void segment_character::set_show_debug_message(bool value)
{
    debug_ = value;
}

void segment_character::binarize_plate()
{
    cv::cvtColor(plate_, hsv_, CV_BGR2HSV);
    cv::split(hsv_, hsv_split_);
    intensity_ = hsv_split_[2];

    constexpr int blockSize = 27;
    constexpr double offset = 5;
    cv::adaptiveThreshold(intensity_, threshold_, 255,
                          cv::ADAPTIVE_THRESH_MEAN_C,
                          cv::THRESH_BINARY_INV,
                          blockSize, offset);
    if(debug_){
        cv::Mat gray;
        cv::cvtColor(plate_, gray, CV_BGR2GRAY);
        cv::Mat gray_thresh;
        cv::adaptiveThreshold(intensity_, gray_thresh, 255,
                              cv::ADAPTIVE_THRESH_MEAN_C,
                              cv::THRESH_BINARY_INV,
                              blockSize, offset);
        cv::imshow("intensity", intensity_);
        cv::imshow("binarize hsv", threshold_);
        cv::imshow("binarize gray", gray_thresh);
        cv::waitKey();
    }
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
        std::cout<<"biry eyes size : "<<plate_.size()<<std::endl;

        cv::imshow("origin plate", input(cv::boundingRect(contour)));
        cv::imshow("rotated point", input_cpy);
        cv::imshow("bird eyes plate", plate_);
        ocv::resize_aspect_ratio(input_cpy, input_cpy, {320, 0});
        cv::imwrite("plate_points.jpg", input_cpy);
        cv::imwrite("origin_plate.jpg", input(cv::boundingRect(contour)));
        cv::imwrite("bird_eyes_plate.jpg", plate_);
        cv::waitKey();
    }
}

void segment_character::
generate_bird_eyes_view(const cv::Mat &input,
                        const segment_character::contour_type &contour)
{    
    cv::RotatedRect const plate_region = cv::minAreaRect(contour);
    cv::Point2f pr_points[4];
    plate_region.points(pr_points);
    ocv::four_points_transform(input, plate_, pr_points);

    bird_eyes_view_debug_message(input, contour, pr_points);
}

/*void segment_character::split_character()
{
    cv::Mat centroids, labels, stats;
    int const nlabels =
            cv::connectedComponentsWithStats(threshold_, labels,
                                             stats, centroids);
}*/
