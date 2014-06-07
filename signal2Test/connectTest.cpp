#include <functional>
#include <boost/signals2.hpp>
#include <boost/thread.hpp>
#include <boost/thread/scoped_thread.hpp>

#include <iostream>

namespace{

void print_something()
{
    std::cout<<"print something : "<<boost::this_thread::get_id()<<std::endl;
}

void worker_thread(boost::signals2::signal<void()> &signal)
{
    std::cout<<"worker thread : "<<boost::this_thread::get_id()<<std::endl;
    signal();
}

void worker_thread2()
{
    std::cout<<"worker thread : "<<boost::this_thread::get_id()<<std::endl;
    boost::signals2::signal<void()> signal;
    signal.connect(print_something);
    signal();
}

}

void connect_from_main_thread()
{
    boost::signals2::signal<void()> signal;
    signal.connect(print_something);
    signal();

    std::cout<<"main thread : "<<boost::this_thread::get_id()<<std::endl;

    boost::strict_scoped_thread<> scoped(boost::thread(
                                             boost::bind(&worker_thread, boost::ref(signal)))
                                         );
}

void connect_from_non_main_thread()
{
    std::cout<<"main thread : "<<boost::this_thread::get_id()<<std::endl;

    boost::strict_scoped_thread<> scoped(boost::thread(
                                             boost::bind(&worker_thread2))
                                         );
}
