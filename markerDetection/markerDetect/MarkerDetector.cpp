#include <algorithm>
#include <iterator>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "markerDetector.hpp"

markerDetector::markerDetector() :
    marker_size_{100, 100},
    marker_2d_corners_{ {0, 0}, {marker_size_.width - 1, 0}, {marker_size_.width - 1, marker_size_.height - 1}, {0, marker_size_.height - 1} },
    min_contour_length_allowed_{100}
{
}

void markerDetector::process_frame(cv::Mat const &input, cv::Mat &output)
{
    to_gray(input, input_gray_);

    threshold(input_gray_, input_threshold_);
    //output = input_threshold_.clone();

    find_contours(input_threshold_, contours_, input_gray_.cols / 15);
    if(input_gray_.size() == output.size() && output.type() == CV_8U){
       output.setTo(0);
    }else{
       output = cv::Mat::zeros(input.size(), CV_8U);
    }
    cv::drawContours(output, contours_, -1, cv::Scalar(255));
}

/**********************************************************
 ****************** implementation ************************
 **********************************************************/

void markerDetector::find_contours(cv::Mat const &input, std::vector<std::vector<cv::Point>> &contours, size_t min_contour_points_allowed)
{   
    cv::findContours(input, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
    auto it = std::partition(std::begin(contours), std::end(contours), [=](std::vector<cv::Point> const &a)
    {
        return a.size() > min_contour_points_allowed;
    });
    contours.erase(it, std::end(contours));
}

void markerDetector::find_marker_candidates(std::vector<std::vector<cv::Point>> const &contours, std::vector<marker> &detected_markers)
{
    // For each contour, analyze if it is likely to be the marker
    is_rectangle(contours);

    //sort the points in anti-clockwise order(top-left, top-right, bottom-right, bottom-left)
    sort_rectangle_points();

    // remove the elements which corners are too close to each other
    remove_too_near_markers();

    // Return candidates
    detected_markers.clear();
    for (size_t i = 0; i < possible_markers_size_; ++i){
        if (!removal_mask_[i])
            detected_markers.emplace_back(std::move(possible_markers_[i]));
    }
}

void markerDetector::is_rectangle(std::vector<std::vector<cv::Point>> const &contours)
{
    approx_curve_.clear();
    possible_markers_.clear();
    size_t const ContoursSize = contours.size();
    for(size_t i = 0; i != ContoursSize; ++i){
        // Approximate to a poligon
        cv::approxPolyDP(contours[i], approx_curve_, contours[i].size() * 0.05, true);

        // We interested only in polygons that contains only four vertices
        if(approx_curve_.size() != 4){
            continue;
        }

        if(!cv::isContourConvex(approx_curve_)){
            continue;
        }

        // Ensure that the distace between consecutive points is large enough
        float min_dist = 1e10;
        for (int i = 0; i < 4; ++i){
            cv::Point const vec = approx_curve_[i] - approx_curve_[(i+1) % 4];
            float const squared_distance = vec.dot(vec);
            min_dist = std::min(min_dist, squared_distance);
        }

        // Check that distance is not very small
        if (min_dist > min_contour_length_allowed_){
            marker m;
            for(int i = 0;i < 4; ++i){
                m.points_.emplace_back(approx_curve_[i].x, approx_curve_[i].y);
            }
            possible_markers_.emplace_back(std::move(m));
        }
    }
}

void markerDetector::remove_too_near_markers()
{
    too_near_candidates_.clear();
    for (size_t i = 0; i < possible_markers_size_; ++i){
        marker const &m1 = possible_markers_[i];

        //calculate the average distance of each corner to the nearest corner of the other marker candidate
        for (size_t j = i + 1; j < possible_markers_size_; ++j){
            marker const &m2 = possible_markers_[j];
            float distSquared = 0;
            for(int c = 0; c < 4; ++c){
                cv::Point v = m1.points_[c] - m2.points_[c];
                distSquared += v.dot(v);
            }
            distSquared /= 4;

            if (distSquared < 100){
                too_near_candidates_.emplace_back(i, j);
            }
        }
    }

    //mark for removal elements of the pair with smaller perimeter
    removal_mask_.resize(possible_markers_size_, false);
    size_t const TooNearCandidatesSize = too_near_candidates_.size();
    for (size_t i = 0; i < TooNearCandidatesSize; ++i){
        float const p1 = cv::arcLength(possible_markers_[too_near_candidates_[i].first ].points_, true);
        float const p2 = cv::arcLength(possible_markers_[too_near_candidates_[i].second].points_, true);

        size_t const removalIndex = p1 > p2 ? too_near_candidates_[i].second : too_near_candidates_[i].first;

        removal_mask_[removalIndex] = true;
    }
}

void markerDetector::sort_rectangle_points()
{
    possible_markers_size_ = possible_markers_.size();
    for (size_t i = 0; i != possible_markers_size_; ++i){
        marker& mark = possible_markers_[i];

        //trace a line between the first and second point.
        //if the thrid point is at the right side, then the points are anti-clockwise
        cv::Point v1 = mark.points_[1] - mark.points_[0];
        cv::Point v2 = mark.points_[2] - mark.points_[0];

        //if the third point is in the left side, then sort in anti-clockwise order
        if ((v1.x * v2.y) - (v1.y * v2.x)  < 0.0) {
            std::swap(mark.points_[1],mark.points_[3]);
        }
    }
}

void markerDetector::threshold(cv::Mat const &gray_input, cv::Mat &output)
{
    cv::threshold(gray_input, output, 0, 255, cv::THRESH_OTSU + cv::THRESH_BINARY_INV);
}

void markerDetector::to_gray(cv::Mat const &input, cv::Mat &out)
{
    switch(input.channels()){
    case 4:{
        cv::cvtColor(input, out, CV_BGRA2GRAY);
        break;
    }
    case 3:{
        cv::cvtColor(input, out, CV_BGR2GRAY);
        break;
    }
    case 1:{
        input.copyTo(out);
        break;
    }
    default:{
        return;
    }
    }
}
