#include "timer00.hpp"

#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/bind.hpp>
#include <iostream>

namespace{

boost::mutex global_stream_lock;

void WorkerThread(boost::shared_ptr< boost::asio::io_service > io_service)
{
    global_stream_lock.lock();
    std::cout << "[" << boost::this_thread::get_id()
              << "] Thread Start" << std::endl;
    global_stream_lock.unlock();

    while( true ){
        try{
            boost::system::error_code ec;
            io_service->run(ec);
            if(ec){
                global_stream_lock.lock();
                std::cout << "[" << boost::this_thread::get_id()
                          << "] Error: " << ec << std::endl;
                global_stream_lock.unlock();
            }
            break;
        }catch(std::exception &ex){
            global_stream_lock.lock();
            std::cout << "[" << boost::this_thread::get_id()
                      << "] Exception: " << ex.what() << std::endl;
            global_stream_lock.unlock();
        }
    }

    global_stream_lock.lock();
    std::cout << "[" << boost::this_thread::get_id()
              << "] Thread Finish" << std::endl;
    global_stream_lock.unlock();
}

void TimerHandler( const boost::system::error_code & error )
{
    if( error ){
        global_stream_lock.lock();
        std::cout << "[" << boost::this_thread::get_id()
                  << "] Error: " << error << std::endl;
        global_stream_lock.unlock();
    }else{
        global_stream_lock.lock();
        std::cout << "[" << boost::this_thread::get_id()
                  << "] TimerHandler " << std::endl;
        global_stream_lock.unlock();
    }
}

}


void timer_00()
{
    boost::shared_ptr< boost::asio::io_service > io_service(
                new boost::asio::io_service
                );
    boost::shared_ptr< boost::asio::io_service::work > work(
                new boost::asio::io_service::work( *io_service )
                );

    global_stream_lock.lock();
    std::cout << "[" << boost::this_thread::get_id()
              << "] Press [return] to exit." << std::endl;
    global_stream_lock.unlock();

    boost::thread_group worker_threads;
    for( int x = 0; x < 2; ++x )
    {
        worker_threads.create_thread( boost::bind( &WorkerThread, io_service ) );
    }

    boost::asio::deadline_timer timer( *io_service );
    timer.expires_from_now( boost::posix_time::seconds( 5 ) );
    timer.async_wait( TimerHandler );

    std::cin.get();

    io_service->stop();

    worker_threads.join_all();
}
