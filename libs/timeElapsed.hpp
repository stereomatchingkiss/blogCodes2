#ifndef TIMEELAPSED_HPP
#define TIMEELAPSED_HPP

#include <chrono>
#include <cmath>

/*
 * calculate elapsed time.
 *
 *@param :
 * ClockType : could be system_clock, steady_clock or high_resolution_clock
 * Ratio  : represents a time interval. ex : milliseconds, microseconds and so on
 * DurationType : return type of the elapsed time
 */
template<typename ClockType = std::chrono::system_clock, typename Ratio = std::milli, typename DurationType = float>
class timeElapsed
{
public:
    explicit timeElapsed() : begin_(ClockType::now())
        {}

    /**
     * @brief   retrieves the elapsed time since the timeElapsed construct
     * @return  the elapsed time since the object construct
     */
    DurationType get_elapsed() const
    {
        return std::chrono::duration<DurationType, Ratio>
               (ClockType::now() - begin_).count();
    }

    static DurationType get_elapsed_static()
    {
        static std::chrono::time_point<ClockType> begin(ClockType::now());

        return std::chrono::duration<DurationType, Ratio>(ClockType::now() - begin).count();
    }

    /**
     * @brief set the initial time after the timeElapsed construct
     * @param begin : the begin time
     */
    void set_begin_time(std::chrono::time_point<ClockType> const begin)
    {
        begin_ = begin;
    }

private:
   std::chrono::time_point<ClockType> begin_;  
};

#endif // TIMEELAPSED_HPP
