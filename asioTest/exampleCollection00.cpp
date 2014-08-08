#include "exampleCollection00.hpp"

#include <boost/asio.hpp>
#include <iostream>

namespace{

void handler(const boost::system::error_code &ec)
{
    std::cout << "5 s." << std::endl;
}

}

void deadline_timer_00()
{
    boost::asio::io_service io_service;
    boost::asio::deadline_timer timer(io_service, boost::posix_time::seconds(5));
    timer.async_wait(handler);
    io_service.run();
}
