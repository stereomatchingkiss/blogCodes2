#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>

//#include <boost/range/adaptors.hpp>
//#include <boost/range/algorithm.hpp>

#include <iostream>

void date_time_example_00()
{
    using namespace boost::gregorian;
    try{
        date today = day_clock::local_day();
        std::cout<<today.year()<<std::endl;
        std::cout<<today.month()<<std::endl;
        std::cout<<today.day()<<std::endl;
        std::cout<<today.day_of_week()<<std::endl;

        date birthday(from_simple_string("2000/01/01"));
        days days_alive = today - birthday;
        std::cout<<days_alive<<std::endl;
    }catch(std::exception const &ex){
        std::cerr<<ex.what()<<std::endl;
    }
}

void date_time_example_01()
{
    typedef boost::posix_time::time_duration::sec_type Sec;

    boost::posix_time::ptime now = boost::posix_time::second_clock::local_time();

    boost::posix_time::time_duration duration = now.time_of_day();
    std::cout<<duration.hours()<<std::endl;
    std::cout<<duration.minutes()<<std::endl;
    std::cout<<duration.seconds()<<std::endl;

    std::cout<<(duration.hours() * 60 * 60 + duration.minutes() * 60 + duration.seconds())<<std::endl;
    std::cout<<duration.total_seconds()<<std::endl;

    boost::posix_time::time_duration t2(12, 34, 55);
    Sec const S1 = duration.total_seconds();
    Sec const S2 = t2.total_seconds();

    std::cout<<(S2 - S1)<<std::endl;
}

int main()
{    
    date_time_example_01();

    return 0;
}

