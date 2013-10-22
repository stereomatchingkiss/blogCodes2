#ifndef MARKERDETECTOR_HPP
#define MARKERDETECTOR_HPP

#include <vector>

#include <opencv2/core/core.hpp>

#include "marker.hpp"

class markerDetector
{
public:
    markerDetector();

    void process_frame(cv::Mat const &input, cv::Mat &output);

private:
    void find_contours(cv::Mat const &input, std::vector<std::vector<cv::Point> > &contours, size_t min_contour_points_allowed);
    void find_marker_candidates(std::vector<std::vector<cv::Point>> const &contours, std::vector<marker> &detected_markers);

    void is_rectangle(std::vector<std::vector<cv::Point>> const &contours);

    void remove_too_near_markers();

    void sort_rectangle_points();

    void threshold(cv::Mat const &gray_input, cv::Mat &output);
    void to_gray(cv::Mat const &input, cv::Mat &out);

private:
    std::vector<cv::Point> approx_curve_;

    std::vector<std::vector<cv::Point>> contours_;

    cv::Mat input_gray_;
    cv::Mat input_threshold_;

    cv::Size const marker_size_;
    std::vector<cv::Point> const marker_2d_corners_;
    float min_contour_length_allowed_;

    std::vector<marker> possible_markers_;
    size_t possible_markers_size_;

    std::vector<bool> removal_mask_;

    std::vector<std::pair<int,int>> too_near_candidates_;
};

#endif // MARKERDETECTOR_HPP
