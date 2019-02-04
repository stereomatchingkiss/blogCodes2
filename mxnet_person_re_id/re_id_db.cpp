#include "re_id_db.hpp"

#include "compare_features_distance.hpp"

#include <libs/mxnet/common.hpp>
#include <libs/mxnet/opencv_to_ndarray.hpp>

#include <mxnet-cpp/MxNetCpp.h>

using namespace mxnet::cpp;
using namespace mxnet_aux;

re_id_db::re_id_db(const std::string &model_params,
                   const std::string &model_symbols,
                   const mxnet::cpp::Context &context) :
    compare_(std::make_unique<cosine_similarity>()),
    to_ndarray_(std::make_unique<mxnet_aux::opencv_to_ndarray>(128, 384, context))
{
    executor_ = create_executor(model_params,
                                model_symbols,
                                context,
                                Shape(1, 128, 384, 3));
}

re_id_db::~re_id_db()
{

}

size_t re_id_db::add_new_id(const cv::Mat &input)
{
    auto data = to_ndarray_->convert(input);
    data.CopyTo(&executor_->arg_dict()["data"]);
    executor_->Forward(false);
    cv::Mat_<float> feature(1, 2048);
    executor_->outputs[0].SyncCopyToCPU(reinterpret_cast<mx_float*>(feature.data), 2048);
    features_.emplace_back(feature);

    return features_.size();
}

re_id_db::id_info re_id_db::find_most_similar_id(const cv::Mat &input) const
{
    id_info result;
    for(size_t i = 0; i != features_.size(); ++i){
        auto const confident = compare_->compare_feature(features_[i], input);
        if(confident > result.confident_){
            result.confident_ = confident;
            result.id_ = i;
        }
    }

    return result;
}
