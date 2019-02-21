#include "opencv_to_ndarray.hpp"

#include <opencv2/imgproc.hpp>

#include <mxnet-cpp/MxNetCpp.h>

using namespace mxnet::cpp;

namespace ocv{

namespace img_cvt{


opencv_to_ndarray::opencv_to_ndarray(int width,
                                     int height,
                                     Context const &context,
                                     int convert_to,
                                     bool switch_channel) :
    context_(context),
    convert_to_(convert_to),
    size_(width, height),
    switch_channel_(switch_channel)
{

}

mxnet::cpp::NDArray opencv_to_ndarray::convert(cv::Mat const &input)
{
    cv::resize(input, resize_img_, size_);
    if(switch_channel_){
        cv::cvtColor(resize_img_, resize_img_, CV_BGR2RGB);
    }
    resize_img_.convertTo(float_img_, convert_to_);

    return NDArray(float_img_.ptr<float>(),
                   Shape(1, static_cast<unsigned>(size_.height), static_cast<unsigned>(size_.width), 3),
                   context_);
}

}

}
