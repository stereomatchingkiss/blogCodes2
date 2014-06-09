#include "asioSignal2Test.hpp"

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/signals2.hpp>
#include <boost/thread.hpp>

#include <iostream>

namespace{

/// @brief io_service dedicated to gui.
boost::asio::io_service gui_service;

/// @brief io_service dedicated to worker.
boost::asio::io_service worker_service;

/// @brief work to keep gui_service from stopping prematurely.
boost::optional<boost::asio::io_service::work> gui_work;

}


void asio_and_signal2_test()
{

}
