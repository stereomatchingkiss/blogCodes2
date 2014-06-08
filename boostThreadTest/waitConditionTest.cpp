#include "waitConditionTest.hpp"

#include <boost/function.hpp>
#include <boost/thread.hpp>
#include <boost/thread/scoped_thread.hpp>

namespace{

boost::mutex global_mutex;
boost::mutex output_lock;
boost::condition_variable wait;
bool ready = false;

inline
bool is_ready()
{
    boost::lock_guard<boost::mutex> lk(output_lock);
    return ready;
}

inline
bool set_ready(bool value)
{
    boost::lock_guard<boost::mutex> lk(output_lock);
    ready = value;
    return ready;
}

void worker_thr()
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

    set_ready(true);
    wait.notify_one();
}

void main_thr()
{
    while(!is_ready()){
        {
            boost::lock_guard<boost::mutex> lk(global_mutex);
            std::cout<<"main thread running : "
                    <<boost::this_thread::get_id()<<std::endl;
        }
        boost::this_thread::sleep( boost::posix_time::milliseconds( 500 ) );
    }
}

void run_func_in_main_thread(boost::function<void()> main_thread,
                             boost::function<void()> worker_thread)
{

    //boost::strict_scoped_thread<> scope(boost::thread(worker_thread));
    boost::thread t1(worker_thread);
    main_thread();
    {
        boost::unique_lock<boost::mutex> lk(global_mutex);
        wait.wait(lk, is_ready);
    }
    std::cout<<"all done"<<std::endl;
}

}

void wait_condition_00()
{
    std::cout<<"main thread id: "
            <<boost::this_thread::get_id()<<std::endl;
    run_func_in_main_thread(main_thr, worker_thr);
}
