#include "getLocalHostAddr.hpp"

#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/bind.hpp>
#include <iostream>

void get_local_host_address()
{
    boost::asio::io_service io;
    boost::asio::ip::tcp::resolver resolver(io);
    boost::asio::ip::tcp::resolver::query query(boost::asio::ip::host_name(), "");
    boost::asio::ip::tcp::resolver::iterator iter = resolver.resolve(query);
    boost::asio::ip::tcp::resolver::iterator end;
    while(iter != end)
    {
        if(iter->endpoint().address().is_v4()){
            std::cout << iter->endpoint().address().to_string() << std::endl;
        }
        ++iter;
    }
}
