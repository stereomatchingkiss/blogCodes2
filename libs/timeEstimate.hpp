#ifndef TIMEESTIMATE_HPP
#define TIMEESTIMATE_HPP

#include <chrono>
#include <cmath>
#include <iostream>

/*
 * output elapse times when destruct, this class
 * is base on std::chrono
 *
 *@param :
 * ClockType : could be system_clock, steady_clock or high_resolution_clock
 * Ratio  : represents a time interval. ex : milliseconds, microseconds and so on
 * DurationType : return type of the elapsed time
 */
template<typename ClockType = std::chrono::steady_clock, typename Ratio = std::milli, typename DurationType = float>
class timeEstimate
{
public:
    explicit timeEstimate(std::string const &message = "Elapsed time : ", double denominator = 1000.0) : begin(ClockType::now()),
        denominator_(std::abs(denominator) + 1.0e-15),
        message_(message) {}
    ~timeEstimate()
    {
        std::chrono::time_point<ClockType> end =
        ClockType::now();

        auto const time = std::chrono::duration<DurationType, Ratio>
                          (end - begin).count();

        std::cout<<message_<<time / denominator_<<std::endl;
    }

private:
   std::chrono::time_point<ClockType> begin;
   double denominator_;
   std::string const message_;
};

#endif // TIMEESTIMATE_HPP
