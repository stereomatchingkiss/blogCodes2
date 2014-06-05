#include "post00.hpp"

#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/bind.hpp>
#include <iostream>

namespace{

void WorkerThread( boost::asio::io_service &io_service, boost::mutex &mutex )
{
    {
        boost::lock_guard<boost::mutex> guard(mutex);
        std::cout << "[" << boost::this_thread::get_id()
                  << "] Thread Start" << std::endl;
    }

    io_service.run(); //without this line, the handler(functor) wouldn't run

    {
        boost::lock_guard<boost::mutex> guard(mutex);
        std::cout << "[" << boost::this_thread::get_id()
                  << "] Thread Finish" << std::endl;
    }
}

size_t fib( size_t n )
{
    if ( n <= 1 )
    {
        return n;
    }
    boost::this_thread::sleep(
                boost::posix_time::milliseconds( 1000 )
                );
    return fib( n - 1 ) + fib( n - 2);
}

void CalculateFib( size_t n, boost::mutex &mutex )
{
    {
        boost::lock_guard<boost::mutex> guard(mutex);
        std::cout << "[" << boost::this_thread::get_id()
                  << "] Now calculating fib( " << n << " ) " << std::endl;
    }

    size_t f = fib( n );

    {
        boost::lock_guard<boost::mutex> guard(mutex);
        std::cout << "[" << boost::this_thread::get_id()
                  << "] fib( " << n << " ) = " << f << std::endl;
    }
}

}

void post_example_00()
{
    boost::asio::io_service io_service;
    boost::asio::io_service::work work(io_service);

    boost::mutex mutex;

    {
        boost::lock_guard<boost::mutex> guard(mutex);
        std::cout << "[" << boost::this_thread::get_id()
                  << "] The program will exit when all work has finished."
                  << std::endl;
    }

    boost::thread_group worker_threads;
    for( int x = 0; x < 2; ++x )
    {
        worker_threads.create_thread(
                    boost::bind( &WorkerThread, boost::ref(io_service),
                                 boost::ref(mutex))
                    );
    }

    io_service.post(boost::bind( CalculateFib, 3, boost::ref(mutex) ));
    io_service.post(boost::bind( CalculateFib, 4, boost::ref(mutex) ));
    io_service.post(boost::bind( CalculateFib, 5, boost::ref(mutex) ));

    //without stop, the io_service.run() will block forever
    io_service.stop();

    //std::cout<<"io service stop"<<std::endl;

    worker_threads.join_all();

    std::cout<<"post_example_00 threads join all"<<std::endl;
}
