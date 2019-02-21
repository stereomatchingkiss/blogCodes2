#include "common.hpp"

#include <mxnet-cpp/MxNetCpp.h>

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

using namespace mxnet::cpp;

namespace ocv{

namespace mxnet_aux{

NDArray cvmat_to_ndarray(cv::Mat const &bgr_image, Context const &ctx)
{    
    cv::Mat rgb_image;
    cv::cvtColor(bgr_image, rgb_image, cv::COLOR_BGR2RGB);
    rgb_image.convertTo(rgb_image, CV_32FC3);
    return NDArray(rgb_image.ptr<float>(),
                   Shape(1, static_cast<unsigned>(rgb_image.rows), static_cast<unsigned>(rgb_image.cols), 3),
                   ctx);
}

NDArray cvmat_to_ndarray(std::string const &filename, Context const &ctx)
{    
    return cvmat_to_ndarray(cv::imread(filename, cv::IMREAD_COLOR), ctx);
}

void load_check_point(std::string const &model_params,
                      std::string const &model_symbol,
                      Symbol *symbol,
                      std::map<std::string, NDArray> *arg_params,
                      std::map<std::string, NDArray> *aux_params,
                      Context const &ctx)
{        
    Symbol new_symbol = Symbol::Load(model_symbol);
    std::map<std::string, NDArray> const params = NDArray::LoadToMap(model_params);
    std::map<std::string, NDArray> args;
    std::map<std::string, NDArray> auxs;
    for (auto const &iter : params) {
        std::string const type = iter.first.substr(0, 4);
        std::string const name = iter.first.substr(4);
        if (type == "arg:")
            args[name] = iter.second.Copy(ctx);
        else if (type == "aux:")
            auxs[name] = iter.second.Copy(ctx);
    }

    *symbol = new_symbol;
    *arg_params = args;
    *aux_params = auxs;

    //WaitAll is need when we copy data between GPU and the main memory
    NDArray::WaitAll();
}

std::unique_ptr<Executor> create_executor(const std::string &model_params,
                                          const std::string &model_symbols,
                                          const Context &context,
                                          const Shape &input_shape)
{    
    Symbol net;
    std::map<std::string, NDArray> args, auxs;
    load_check_point(model_params, model_symbols, &net, &args, &auxs, context);

    //The shape of the input data must be the same, if you need different size,
    //you could rebind the Executor or create a pool of Executor.
    //In order to create input layer of the Executor, I make a dummy NDArray.
    //The value of the "data" could be change later
    args["data"] = NDArray(input_shape, context, false);
    if(input_shape[0] > 1){
        args["data1"] = NDArray(Shape(1), context, false);
    }

    std::unique_ptr<Executor> executor;
    executor.reset(net.SimpleBind(context, args, std::map<std::string, NDArray>(),
                                  std::map<std::string, OpReqType>(), auxs));

    return executor;
}

}

}
