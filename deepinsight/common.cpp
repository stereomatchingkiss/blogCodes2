#include "common.hpp"

#include <mxnet-cpp/MxNetCpp.h>

#include <iostream>

using namespace std;

using namespace mxnet::cpp;

namespace mxnet_tool{

void load_check_point(std::string const &model_params,
                      std::string const &model_symbol,
                      Symbol *symbol,
                      std::map<std::string, NDArray> *arg_params,
                      std::map<std::string, NDArray> *aux_params,
                      Context const &ctx)
{
    Symbol new_symbol = Symbol::Load(model_symbol);    
    std::map<std::string, NDArray> args;
    std::map<std::string, NDArray> auxs;
    for(auto iter : NDArray::LoadToMap(model_params)){
        std::string type = iter.first.substr(0, 4);
        std::string name = iter.first.substr(4);
        if (type == "arg:"){
            args[name] = iter.second.Copy(ctx);
        }else if (type == "aux:"){
            auxs[name] = iter.second.Copy(ctx);
        }
    }

    *symbol = new_symbol;
    *arg_params = args;
    *aux_params = auxs;
}

}
