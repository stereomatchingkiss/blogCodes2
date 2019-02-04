#include "person_feautres_extractor.hpp"

#include <mxnet-cpp/MxNetCpp.h>

#include <opencv2/imgproc.hpp>

#include <libs/mxnet/common.hpp>
#include <libs/mxnet/opencv_to_ndarray.hpp>

using namespace mxnet::cpp;
using namespace mxnet_aux;

namespace{

int constexpr input_height = 384;
int constexpr input_width = 128;

}

person_feautres_extractor::person_feautres_extractor(const std::string &model_params,
                                                     const std::string &model_symbols,
                                                     const mxnet::cpp::Context &context)
{
    executor_ = create_executor(model_params, model_symbols, context,
                                Shape(1, input_height, input_width, 3));
    to_ndarray_ = std::make_unique<opencv_to_ndarray>(input_width, input_height, context, CV_32FC3, false);
}

person_feautres_extractor::~person_feautres_extractor()
{

}

cv::Mat_<float> person_feautres_extractor::get_features(const cv::Mat &input)
{
    auto data = to_ndarray_->convert(input);
    data.CopyTo(&executor_->arg_dict()["data"]);
    executor_->Forward(false);

    cv::Mat_<float> result(1, 2048);
    if(!executor_->outputs.empty()){
        executor_->outputs[0].SyncCopyToCPU(result.ptr<float>(), 2048);
    }

    return result;
}

