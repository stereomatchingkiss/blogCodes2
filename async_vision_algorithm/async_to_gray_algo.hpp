#ifndef ASYNC_TO_GRAY_ALGO_HPP
#define ASYNC_TO_GRAY_ALGO_HPP

#include <opencv2/core.hpp>

#include <functional>
#include <thread>

class async_to_gray_algo
{
public:
    explicit async_to_gray_algo(cv::Mat &result, std::mutex &gmutex);
    ~async_to_gray_algo();

    void process(cv::Mat input);

private:    
    cv::Mat input_;
    std::mutex mutex_;
    cv::Mat &result_;
    std::mutex &result_mutex_;
    bool stop_;
    std::thread thread_;
    std::condition_variable wait_;
};

#endif // ASYNC_TO_GRAY_ALGO_HPP
