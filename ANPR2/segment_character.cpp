#include "segment_character.hpp"

#include <ocv_libs/core/perspective_transform.hpp>

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
                  const segment_character::Countours &contours)
{
    generate_bird_eyes_view(input, contours);

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

void segment_character::
generate_bird_eyes_view(const cv::Mat &input,
                        const segment_character::Countours &contours)
{
    plate_.clear();
    for(auto const &contour : contours){
        cv::RotatedRect const plate_region = cv::minAreaRect(contour);
        cv::Point2f pr_points[4];
        plate_region.points(pr_points);
        auto bird_eyes_plate = ocv::four_points_transform(input, pr_points);
        plate_.emplace_back(bird_eyes_plate);
        if(debug_){
            auto input_cpy = input.clone();
            ocv::sort_corners(pr_points, std::begin(pr_points));
            auto const center = ocv::corners_center(pr_points);
            std::cout<<"center : "<<center<<std::endl;
            cv::circle(input_cpy, center, 3, {0, 0, 255}, 2);
            for(size_t i = 0; i != 4; ++i){
                //std::cout<<pt<<", x="<<pt.x<<", y="<<pt.y<<std::endl;
                std::cout<<i<<" : "<<pr_points[i]<<std::endl;
                auto text_point = pr_points[i];
                text_point.y -= 10;
                double const scale = 1.0;
                int const thickness = 2;
                cv::putText(input_cpy, std::to_string(i), text_point,
                            cv::FONT_HERSHEY_COMPLEX, scale, {255,0,0}, thickness);
                int const radius = 3;
                cv::circle(input_cpy, pr_points[i], radius,
                {0, 255, 0}, thickness);
            }
            std::cout<<"origin size : "<<input(cv::boundingRect(contour)).size()<<std::endl;
            std::cout<<"biry eyes size : "<<bird_eyes_plate.size()<<std::endl;

            cv::imshow("origin plate", input(cv::boundingRect(contour)));
            cv::imshow("rotated point", input_cpy);
            cv::imshow("bird eyes plate", plate_.back());
            cv::waitKey();
        }
    }
}
