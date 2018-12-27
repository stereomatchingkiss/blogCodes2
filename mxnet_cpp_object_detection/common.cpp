#include "common.hpp"

#include <mxnet-cpp/MxNetCpp.h>

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

using namespace mxnet::cpp;

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
}

std::vector<std::string> create_coco_obj_detection_labels()
{
    return {
        "person", "bicycle", "car", "motorcycle", "airplane", "bus", "train",
        "truck", "boat", "traffic light", "fire hydrant", "stop sign",
        "parking meter", "bench", "bird", "cat", "dog", "horse", "sheep",
        "cow", "elephant", "bear", "zebra", "giraffe", "backpack", "umbrella",
        "handbag", "tie", "suitcase", "frisbee", "skis", "snowboard",
        "sports ball", "kite", "baseball bat", "baseball glove", "skateboard",
        "surfboard", "tennis racket", "bottle", "wine glass", "cup", "fork",
        "knife", "spoon", "bowl", "banana", "apple", "sandwich", "orange",
        "broccoli", "carrot", "hot dog", "pizza", "donut", "cake", "chair",
        "couch", "potted plant", "bed", "dining table", "toilet", "tv",
        "laptop", "mouse", "remote", "keyboard", "cell phone", "microwave",
        "oven", "toaster", "sink", "refrigerator", "book", "clock", "vase",
        "scissors", "teddy bear", "hair drier", "toothbrush"
    };
}
