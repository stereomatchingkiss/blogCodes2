#include "fps_estimator.hpp"

#include <chrono>

void fps_estimator::end()
{
    std::chrono::duration<double> const elapsed_seconds = std::chrono::system_clock::now() - start_;
    elapsed_ += elapsed_seconds.count();
}

double fps_estimator::get_elapsed() const noexcept
{
    return elapsed_;
}

int fps_estimator::get_total_frame() const noexcept
{
    return total_frame_;
}

double fps_estimator::get_fps() const noexcept
{
    return elapsed_ != 0.0 ? total_frame_/elapsed_ : 0.0;
}

void fps_estimator::start()
{
    if(total_frame_ == std::numeric_limits<int>::max()){
       total_frame_ = 0;
       elapsed_ = 0;
    }
    ++total_frame_;
    start_ = std::chrono::system_clock::now();
}
