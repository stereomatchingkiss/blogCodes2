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
        auto bird_eyes_plate =
                ocv::four_points_transform(input, pr_points);
        plate_.emplace_back(bird_eyes_plate);
        if(debug_){
            cv::imshow("bird eyes plate", plate_.back());
            cv::waitKey();
        }
    }
}
