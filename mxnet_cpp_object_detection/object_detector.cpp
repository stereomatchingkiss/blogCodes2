#include "object_detector.hpp"

#include <mxnet-cpp/MxNetCpp.h>

#include <opencv2/imgproc.hpp>

#include "common.hpp"

using namespace mxnet::cpp;

object_detector::object_detector(std::string const &model_params,
                                 std::string const &model_symbols,
                                 Context const &context,
                                 cv::Size const &process_size) :
    context_(new Context(context.GetDeviceType(), context.GetDeviceId())),
    process_size_(process_size)
{    
    Symbol net;
    std::map<std::string, NDArray> args, auxs;
    load_check_point(model_params, model_symbols, &net, &args, &auxs, context);

    //The shape of the input data must be the same, if you need different size,
    //you could rebind the Executor or create a pool of Executor.
    //In order to create input layer of the Executor, I make a dummy NDArray.
    //The value of the "data" could be change later
    args["data"] = NDArray(Shape(1, static_cast<unsigned>(process_size.height),
                                 static_cast<unsigned>(process_size.width), 3), context);
    executor_.reset(net.SimpleBind(context, args, std::map<std::string, NDArray>(),
                                   std::map<std::string, OpReqType>(), auxs));
}

object_detector::~object_detector()
{

}

void object_detector::forward(const cv::Mat &input)
{
    //By default, input_size_.height equal to 256 input_size_.width equal to 320.
    //Yolo v3 has a limitation, width and height of the image must be divided by 32.
    if(input.rows != process_size_.height || input.cols != process_size_.width){
        cv::resize(input, resize_img_, process_size_);
    }else{
        resize_img_ = input;
    }

    auto data = cvmat_to_ndarray(resize_img_, *context_);
    //Copy the data of the image to the "data"
    data.CopyTo(&executor_->arg_dict()["data"]);    
    //Forward is an async api.
    executor_->Forward(false);
}

mxnet::cpp::NDArray object_detector::get_bboxes() const
{
    return executor_->outputs[2].Copy(Context::cpu());
}

mxnet::cpp::NDArray object_detector::get_labels() const
{
    return executor_->outputs[0].Copy(Context::cpu());
}

std::vector<NDArray> object_detector::get_outputs() const
{
    return executor_->outputs;
}

mxnet::cpp::NDArray object_detector::get_scores() const
{
    return executor_->outputs[1].Copy(Context::cpu());
}
