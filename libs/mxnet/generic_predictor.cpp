#include "generic_predictor.hpp"

#include "common.hpp"

#include <mxnet-cpp/MxNetCpp.h>

using namespace mxnet::cpp;

namespace ocv{

namespace mxnet_aux{

generic_predictor::generic_predictor(const std::string &model_params,
                                     const std::string &model_symbol,
                                     const Context &context,
                                     const Shape &shape)
{
    executor_ = mxnet_aux::create_executor(model_params, model_symbol, context, shape);
    image_vector_.resize(shape.Size());
    max_batch_size_ = shape[0];
}

generic_predictor::~generic_predictor()
{

}

}

}
