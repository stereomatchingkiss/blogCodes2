#include "face_key_extractor.hpp"

#include "common.hpp"
#include "face_key.hpp"

#include <opencv2/imgproc.hpp>

#include <mxnet-cpp/MxNetCpp.h>

using namespace mxnet::cpp;

namespace mxnet_tool{

namespace{

NDArray dlib_matrix_to_ndarray(dlib::matrix<dlib::rgb_pixel> const &rgb_image, Context const &ctx)
{
    std::vector<float> buffer(rgb_image.nc() * rgb_image.nr() * 3);
    size_t index = 0;
    for(size_t ch = 0; ch != 3; ++ch){
        for(size_t row = 0; row != rgb_image.nr(); ++row){
            for(size_t col = 0; col != rgb_image.nc(); ++col){
                auto const &pix = rgb_image(row, col);
                switch(ch){
                case 0:
                    buffer[index++] = pix.red;
                    break;
                case 1:
                    buffer[index++] = pix.green;
                    break;
                case 2:
                    buffer[index++] = pix.blue;
                    break;
                default:
                    break;
                }
            }
        }
    }
    return NDArray(&buffer[0], Shape(1, 3, rgb_image.nr(), rgb_image.nc()), ctx);
}

}

face_key_extractor::face_key_extractor(std::string const &model_params,
                                       std::string const &model_symbols,
                                       mxnet::cpp::Context const &context) :
    context_(new Context(context.GetDeviceType(), context.GetDeviceId()))
{
    Symbol net;
    std::map<std::string, NDArray> args, auxs;
    load_check_point(model_params, model_symbols, &net, &args, &auxs, context);

    //The shape of the input data must be the same, if you need different size,
    //you could rebind the Executor or create a pool of Executor.
    //In order to create input layer of the Executor, I make a dummy NDArray.
    //The value of the "data" could be change later
    args["data"] = NDArray(Shape(1, 3, 112, 112), context);    
    executor_.reset(net.SimpleBind(context,
                                   args,
                                   std::map<std::string, NDArray>(),
                                   std::map<std::string, OpReqType>(),
                                   auxs));
}

face_key face_key_extractor::forward(const dlib::matrix<dlib::rgb_pixel> &input)
{
    auto const data = dlib_matrix_to_ndarray(input, *context_);    
    return forward(data);
}

face_key face_key_extractor::forward(const cv::Mat &input)
{
    cv::Mat resize_img;
    cv::resize(input, resize_img, cv::Size(112, 112));
    cv::Mat flat_image(resize_img.rows, resize_img.cols, CV_32FC3);
    auto *flat_ptr = flat_image.ptr<float>(0, 0);
    for(int ch = 0; ch != 3; ++ch){
        for(int row = 0; row != flat_image.rows; ++row){
            for(int col = 0; col != flat_image.cols; ++col){
                auto const &vec = resize_img.at<cv::Vec3b>(row, col);
                *flat_ptr = vec[ch];
                ++flat_ptr;
            }
        }
    }

    auto const data = NDArray(flat_image.ptr<float>(), Shape(1, 3, flat_image.rows, flat_image.cols), *context_);
    return forward(data);
}

face_key face_key_extractor::forward(const NDArray &input)
{
    input.CopyTo(&executor_->arg_dict()["data"]);    
    executor_->Forward(false);
    if(!executor_->outputs.empty()){
        auto features = executor_->outputs[0].Copy(Context(kCPU, 0));
        features.WaitToRead();
        cv::Mat temp(1, 512, CV_32F, const_cast<mx_float*>(features.GetData()), 0);
        cv::normalize(temp, temp, 1, 0, cv::NORM_L2);
        return face_key(features);
    }

    return face_key(NDArray());
}

}
