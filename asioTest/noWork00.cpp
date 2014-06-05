#include "noWork00.hpp"

#include <boost/asio.hpp>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/bind.hpp>
#include <iostream>

namespace{

void worker_thread(boost::asio::io_service &io_service, boost::mutex &global_stream_lock)
{
    global_stream_lock.lock();
    std::cout << "[" << boost::this_thread::get_id() <<
        "] Thread Start" << std::endl;
    global_stream_lock.unlock();

    //if we comment out this line, io_service would not
    //block the execution
    io_service.run(); //#1

    global_stream_lock.lock();
    std::cout << "[" << boost::this_thread::get_id() <<
        "] Thread Finish" << std::endl;
    global_stream_lock.unlock();
}

}

void no_work_00()
{
    boost::asio::io_service io_service;
    //if we remove the work, there will be no "work" for
    //asio to do, so when we call #1 in the worker_thread
    //function, the worker_thread function would not be blocked
    boost::asio::io_service::work work(io_service);

    boost::mutex global_stream_lock;
    global_stream_lock.lock();
    std::cout << "[" << boost::this_thread::get_id()
        << "] Press [return] to exit." << std::endl;
    global_stream_lock.unlock();

    boost::thread_group worker_threads;
    for( int x = 0; x < 4; ++x )
    {
        worker_threads.create_thread( boost::bind( &worker_thread, boost::ref(io_service),
                                      boost::ref(global_stream_lock)));
    }

    std::cin.get();

    io_service.stop();

    worker_threads.join_all();
}

