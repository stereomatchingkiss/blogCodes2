#include "waitConditionTest.hpp"

#define BOOST_THREAD_PROVIDES_FUTURE

#include <boost/function.hpp>
#include <boost/thread.hpp>
#include <boost/thread/future.hpp>

namespace{

boost::mutex global_mutex;

int worker_thr()
{
    size_t num = 0;
    while(num < 10){
        {
            boost::lock_guard<boost::mutex> lk(global_mutex);
            std::cout<<num<<" : worker thread running : "
                    <<boost::this_thread::get_id()<<std::endl;
        }
        boost::this_thread::sleep( boost::posix_time::milliseconds( 500 ) );
        ++num;
    }

    return num;
}

void main_thr()
{    
    {
        boost::lock_guard<boost::mutex> lk(global_mutex);
        std::cout<<"main thread running : "
                <<boost::this_thread::get_id()<<std::endl;
    }
    boost::this_thread::sleep( boost::posix_time::milliseconds( 500 ) );
}

void print_value(int value)
{
    std::cout<<"value : "<<value<<std::endl;
}

void run_func_in_main_thread(boost::function<void()> main_thread,
                             boost::function<int()> worker_thread)
{
    auto future = boost::async(boost::launch::async, worker_thread);

    while(!future.has_value()){
        main_thread();
    }

    print_value(future.get());
    std::cout<<"all done"<<std::endl;
}

}

void wait_condition_00()
{
    std::cout<<"main thread id: "
            <<boost::this_thread::get_id()<<std::endl;
    run_func_in_main_thread(main_thr, worker_thr);
}
