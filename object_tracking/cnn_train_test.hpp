#ifndef CNN_TRAIN_TEST_HPP
#define CNN_TRAIN_TEST_HPP

//#include <mlpack/core.hpp>

#include <opencv2/core.hpp>

#include <vector>

class cnn_train_test
{
public:
    cnn_train_test();

    void train_test_00();

private:
    void read_file();

    std::vector<cv::Mat> positive_data_;
    std::vector<cv::Mat> negative_data_;
};

#endif CNN_TRAIN_TEST_HPP
