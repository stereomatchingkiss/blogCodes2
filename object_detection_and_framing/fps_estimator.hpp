#ifndef FPS_ESTIMATOR_HPP
#define FPS_ESTIMATOR_HPP

#include <chrono>

class fps_estimator
{
public:
    void end();
    double get_elapsed() const noexcept;
    int get_total_frame() const noexcept;
    double get_fps() const noexcept;

    void start();    

private:
    double elapsed_ = 0.0;    
    std::chrono::time_point<std::chrono::system_clock> start_;
    int total_frame_ = 0;
};

#endif // FPS_ESTIMATOR_HPP
