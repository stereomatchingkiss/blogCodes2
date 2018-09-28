#ifndef COMMON_HPP
#define COMMON_HPP

#include <mxnet-cpp/ndarray.h>

#include <opencv2/core.hpp>

#include <string>
#include <map>

namespace mxnet{

namespace cpp{

class Context;
class Symbol;

}

}

std::vector<std::string> create_coco_obj_detection_labels();

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

#endif // COMMON_HPP
