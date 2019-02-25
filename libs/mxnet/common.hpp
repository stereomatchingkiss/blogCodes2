#ifndef OCV_MXNET_AUX_COMMON_HPP
#define OCV_MXNET_AUX_COMMON_HPP

#include <mxnet-cpp/ndarray.h>

#include <opencv2/core.hpp>

#include <map>
#include <memory>
#include <string>

namespace mxnet{

namespace cpp{

class Context;
class Executor;
class Symbol;

}

}

namespace ocv{

namespace mxnet_aux{

/**
 * @param input_shape batch, height, width, channels
 */
std::unique_ptr<mxnet::cpp::Executor>
create_executor(std::string const &model_params,
                std::string const &model_symbols,
                mxnet::cpp::Context const &context,
                mxnet::cpp::Shape const &input_shape);

// Load data from CV BGR image
mxnet::cpp::NDArray cvmat_to_ndarray(cv::Mat const &bgr_image, mxnet::cpp::Context const &ctx);

// Load data from filename
mxnet::cpp::NDArray cvmat_to_ndarray(std::string const &filename, mxnet::cpp::Context const &ctx);

void load_check_point(std::string const &model_params,
                      std::string const &model_symbol,
                      mxnet::cpp::Symbol *symbol,
                      std::map<std::string, mxnet::cpp::NDArray> *arg_params,
                      std::map<std::string, mxnet::cpp::NDArray> *aux_params,
                      mxnet::cpp::Context const &ctx);

}

}

#endif // OCV_MXNET_AUX_COMMON_HPP
