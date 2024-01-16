#include "fps_estimator.hpp"

#include <chrono>

namespace flt::utils{

void fps_estimator::clear()
{
    total_frame_ = 0;
    elapsed_ = 0;
}

void fps_estimator::end()
{
    if(total_frame_ > warm_up_count_){
        std::chrono::duration<double> const elapsed_seconds = std::chrono::system_clock::now() - start_;
        elapsed_ += elapsed_seconds.count();
    }
}

std::string fps_estimator::get_details() const
{
    return "fps = " + std::to_string(get_fps()) + ", elapsed(secs) = " + std::to_string(elapsed_) +
            ", total frame = " + std::to_string(get_total_frame());
}

double fps_estimator::get_elapsed() const noexcept
{
    return elapsed_;
}

int fps_estimator::get_total_frame() const noexcept
{
    return total_frame_ > warm_up_count_ ? total_frame_ - warm_up_count_ : 0;
}

double fps_estimator::get_fps() const noexcept
{
    return elapsed_ > 0.0 ? get_total_frame()/elapsed_ : 0.0;
}

void fps_estimator::set_warm_up_count(int val)
{
    warm_up_count_ = val;
}

void fps_estimator::start()
{
    if(total_frame_ == std::numeric_limits<int>::max()){
        clear();
    }
    ++total_frame_;
    if(total_frame_ > warm_up_count_){
        start_ = std::chrono::system_clock::now();
    }
}

}
