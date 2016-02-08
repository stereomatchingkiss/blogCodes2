#ifndef TINY_CNN_CAR_CLASSIFIER_HPP
#define TINY_CNN_CAR_CLASSIFIER_HPP

#include <opencv2/core.hpp>

#include <tiny_cnn/tiny_cnn.h>

class tiny_cnn_car_classifier
{
public:
    tiny_cnn_car_classifier();

    tiny_cnn::vec_t predict(cv::Mat const &input);

private:
    using network_type =
    tiny_cnn::network<tiny_cnn::mse, tiny_cnn::adagrad>;

    cv::Mat gray_img_;
    network_type nn_;
    cv::Mat resize_img_;
    tiny_cnn::vec_t vec_;
};

#endif // TINY_CNN_CAR_CLASSIFIER_HPP
