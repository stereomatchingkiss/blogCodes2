#ifndef OCV_IMG_CVT_OPENCV_TO_NDARRAY_HPP
#define OCV_IMG_CVT_OPENCV_TO_NDARRAY_HPP

#include <mxnet-cpp/MxNetCpp.h>

#include <opencv2/core.hpp>

#include <memory>

namespace ocv{

namespace img_cvt{

/**
 * A more efficient way to convert Mat to NDArray since it will reuse allocated memory
 */
class opencv_to_ndarray
{
public:
    /**
     * @param width Width of NDArray
     * @param height Height of NDArray
     * @param convert_to Ex : CV_32FC3
     * @param switch_channel : True will switch R and B channel
     */
    opencv_to_ndarray(int width,
                      int height,
                      mxnet::cpp::Context const &context,
                      int convert_to = CV_32FC3,
                      bool switch_channel = true);

    mxnet::cpp::NDArray convert(cv::Mat const &input);

private:
    mxnet::cpp::Context context_;
    int convert_to_;
    cv::Mat float_img_;
    cv::Mat resize_img_;
    cv::Size size_;
    bool switch_channel_;
};

}

}

#endif // OCV_IMG_CVT_OPENCV_TO_NDARRAY_HPP
