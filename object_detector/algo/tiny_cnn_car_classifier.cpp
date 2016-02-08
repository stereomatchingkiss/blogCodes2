#include "tiny_cnn_car_classifier.hpp"

#include <opencv2/imgproc.hpp>

#include <ocv_libs/tiny_cnn/image_converter.hpp>

#include <fstream>

tiny_cnn_car_classifier::tiny_cnn_car_classifier()
{
    using namespace tiny_cnn;
    using namespace tiny_cnn::activation;

    using activate = relu;

    nn_ << convolutional_layer<activate>(32, 32, 3, 1, 12, padding::same)
       << max_pooling_layer<activate>(32, 32, 12, 2)
       << convolutional_layer<activate>(16, 16, 3, 12, 18, padding::same)
       << max_pooling_layer<activate>(16, 16, 18, 2)
       << convolutional_layer<activate>(8, 8, 5, 18, 24)
       << fully_connected_layer<softmax>(4*4*24, 2);

    std::ifstream in("car_weights_lenet0_c1_1951_2000_8144_v_h");
    in>>nn_;
}

tiny_cnn::vec_t tiny_cnn_car_classifier::
predict(const cv::Mat &input)
{
    cv::resize(input, resize_img_, {32,32});
    cv::cvtColor(resize_img_, gray_img_, CV_BGR2GRAY);
    ocv::tiny_cnn::cvmat_to_img(gray_img_, vec_);

    return nn_.predict(vec_);
}
