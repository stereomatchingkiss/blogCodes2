#pragma once

#include <chrono>
#include <string>

namespace flt::utils{

/**
 * Estimate fps of algorithm
 * @code
 * while(video.not_at_end()){
 *     fps_est_->start();
 *     //do something.......
 *     cv::putText(inout, "fps: " + std::to_string(fps_est_->get_fps()),
 *                 cv::Point(0, inout.rows - 40), cv::FONT_HERSHEY_SIMPLEX, 1, {255,0,255}, 2);
 *     fps_est_->end();
 * }
 * @endcode
 */
class fps_estimator
{
public:
    void clear();
    void end();
    std::string get_details() const;
    double get_elapsed() const noexcept;
    int get_total_frame() const noexcept;
    double get_fps() const noexcept;

    void set_warm_up_count(int val);
    void start();    

private:    
    double elapsed_ = 0.0;
    std::chrono::time_point<std::chrono::system_clock> start_;
    int total_frame_ = 0;
    int warm_up_count_ = 10;
};

}
