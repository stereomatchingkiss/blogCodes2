#include "waitFutureTest.hpp"

#define BOOST_THREAD_PROVIDES_FUTURE

#include <boost/function.hpp>
#include <boost/thread.hpp>
#include <boost/thread/future.hpp>

namespace{

int calculate_the_answer_to_life_the_universe_and_everything()
{
    std::cout<<"calculate_the_answer thread id : "<<boost::this_thread::get_id()<<std::endl;
    return 42;
}

void invoke_lazy_task(boost::packaged_task<int>& task)
{
    try
    {
        task();
    }
    catch(boost::task_already_started&)
    {}
}

}

void wait_future_test_00()
{
    std::cout<<"main thread id : "<<boost::this_thread::get_id()<<std::endl;

    boost::packaged_task<int> task(calculate_the_answer_to_life_the_universe_and_everything);
    task.set_wait_callback(invoke_lazy_task);
    boost:: future<int> f(task.get_future());

    std::cout<<f.get()<<std::endl;
}
