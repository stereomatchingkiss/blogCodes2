#include "async_to_gray_algo.hpp"

#include <opencv2/imgproc.hpp>

#include <iostream>

using namespace cv;
using namespace std;

async_to_gray_algo::async_to_gray_algo(cv::Mat &result, std::mutex &result_mutex) :
    result_(result),
    result_mutex_(result_mutex),
    stop_(false)
{
    auto func = [&]()
    {
        //1. In order to reuse the thread, we need to keep it alive
        //that is why we should put it in an infinite for loop
        for(;;){
            unique_lock<mutex> lock(mutex_);
            //2. use condition_variable to replace sleep(x milliseconds) is more efficient
            wait_.wait(lock, [&]() //wait_ will acquire the lock if condition satisfied
            {
                return stop_ || !input_.empty();
            });
            //3. stop the thread in destructor
            if(stop_){
                return;
            }

            //4. convert and write the results into result_
            //we need gmutex to synchronize the result_, else it may incur
            //race condition in the main thread.
            {
                lock_guard<mutex> glock(result_mutex);
                cv::cvtColor(input_, result_, COLOR_BGR2GRAY);
            }
            //5: clear the input_, else the wait_ variable may wake up and continue the task
            //due to spurious wake up
            input_.resize(0);
        }
    };
    thread_ = std::thread(func);
}

async_to_gray_algo::~async_to_gray_algo()
{
    {
        lock_guard<mutex> lock(mutex_);
        stop_ = true;
    }
    wait_.notify_one();
    thread_.join();
}

void async_to_gray_algo::process(Mat input)
{
    {
        lock_guard<mutex> lock(mutex_);
        input_ = input;
    }
    wait_.notify_one();
}
