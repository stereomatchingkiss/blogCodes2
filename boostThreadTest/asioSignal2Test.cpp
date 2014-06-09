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

boost::optional<boost::asio::io_service::work> worker_work;

//boost::signals2::signal<void()> signal;

void hello_word()
{
    std::cout<<"id of hello world : "<<boost::this_thread::get_id()<<std::endl;
    std::cout<<"hello world"<<std::endl;
}

void call_gui_thread_from_worker()
{
    std::cout<<"id of this thread : "<<boost::this_thread::get_id()<<std::endl;

    //signal();

    //gui_service.post();
    gui_service.post(hello_word);
    //worker_service.stop();
}

void worker_main()
{
    std::cout<<"worker thread id : "<<boost::this_thread::get_id()<<std::endl;
    worker_service.run();
}

}


void asio_and_signal2_test()
{
    //signal.connect(&hello_word);

    gui_work = boost::in_place(boost::ref(gui_service));
    worker_work = boost::in_place(boost::ref(worker_service));

    std::cout << "GUI thread: " << boost::this_thread::get_id() << std::endl;

    // Spawn off worker thread.
    boost::thread worker_thread(worker_main);

    // Add work to worker.
    worker_service.post(call_gui_thread_from_worker);

    gui_service.run();
    std::cout<<"after gui service run"<<std::endl;
    worker_work = boost::none;
    //gui_service.stop();
    gui_work = boost::none;

    worker_thread.join();
}
