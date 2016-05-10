#ifndef CORRELATION_TRACKERS_HPP
#define CORRELATION_TRACKERS_HPP

#include <opencv2/core.hpp>

#include <dlib/image_processing.h>

#include <vector>

/**
 * track the object based on dlib::correlation_tracker
 */
class correlation_tracker
{
public:
    /**
     * add a new tracker
     * @param input input image with target want to track
     * @param roi roi of the target
     */
    void add_track(cv::Mat const &input,
                   cv::Rect const &roi);

    void clear();

    bool empty() const;

    cv::Rect get_position(size_t target) const;
    std::vector<cv::Rect> get_position() const;
    size_t get_track_size() const;

    void update(cv::Mat const &input);

private:
    using tracker_type = dlib::correlation_tracker;

    cv::Mat img_;
    cv::Rect position_;
    std::vector<tracker_type> trackers_;
};

#endif //CORRELATION_TRACKERS_HPP
