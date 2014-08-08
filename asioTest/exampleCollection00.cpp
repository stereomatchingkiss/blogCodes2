#include "exampleCollection00.hpp"

#include <boost/asio.hpp>

#include <array>
#include <functional>
#include <iostream>

namespace{

void handler(const boost::system::error_code &)
{
    std::cout << "5 s." << std::endl;
}

void read_handler(const boost::system::error_code &ec,
                  std::size_t bytes_transferred,
                  std::array<char, 4096> &data,
                  boost::asio::ip::tcp::socket &sock)
{
    namespace ph = std::placeholders;

    if(!ec){
        std::cout << std::string(data.data(), bytes_transferred) << std::endl;
        sock.async_read_some(boost::asio::buffer(data),
                             std::bind(read_handler,
                                       ph::_1, ph::_2,
                                       std::ref(data),
                                       std::ref(sock)));
    }
}

void connect_handler(const boost::system::error_code &ec,
                     std::array<char, 4096> &data,
                     boost::asio::ip::tcp::socket &sock)
{
    namespace ph = std::placeholders;

    if (!ec){
        boost::asio::write(sock, boost::asio::buffer("give me back my money"));
        sock.async_read_some(boost::asio::buffer(data),
                             std::bind(read_handler,
                                       ph::_1, ph::_2,
                                       std::ref(data),
                                       std::ref(sock)));
    }
}

void resolve_handler(const boost::system::error_code &ec,
                     boost::asio::ip::tcp::resolver::iterator it,
                     std::array<char, 4096> &data,
                     boost::asio::ip::tcp::socket &sock)
{
    namespace ph = std::placeholders;

    if (!ec){
        sock.async_connect(*it, std::bind(connect_handler, ph::_1,
                                          std::ref(data),
                                          std::ref(sock)));
    }
}

}

void deadline_timer_00()
{
    boost::asio::io_service io_service;
    boost::asio::deadline_timer timer(io_service, boost::posix_time::seconds(5));
    timer.async_wait(handler);
    io_service.run();
}


void connect_to_local_host()
{
    namespace ph = std::placeholders;

    boost::asio::io_service io_service;
    boost::asio::ip::tcp::resolver resolver(io_service);
    boost::asio::ip::tcp::socket sock(io_service);
    std::array<char, 4096> data;

    boost::asio::ip::tcp::resolver::query query("127.0.0.1", "9999");
    resolver.async_resolve(query, std::bind(resolve_handler,
                                            ph::_1, ph::_2,
                                            std::ref(data),
                                            std::ref(sock)));
    io_service.run();
}
