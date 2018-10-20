#ifndef COMMON_HPP
#define COMMON_HPP

#include <mxnet-cpp/ndarray.h>

#include <string>
#include <map>

namespace mxnet{

namespace cpp{

class Context;
class Symbol;

}

}

namespace mxnet_tool{

void load_check_point(std::string const &model_params,
                      std::string const &model_symbol,
                      mxnet::cpp::Symbol *symbol,
                      std::map<std::string, mxnet::cpp::NDArray> *arg_params,
                      std::map<std::string, mxnet::cpp::NDArray> *aux_params,
                      mxnet::cpp::Context const &ctx);

}


#endif // COMMON_HPP
