#include "insight_age_gender_predict.hpp"

#include "insight_age_gender_predict_params.hpp"

#include "../libs/image_format_convert/dlib_to_array.hpp"
#include "../libs/mxnet/common.hpp"

using namespace mxnet::cpp;

namespace ocv{

namespace face{

insight_age_gender_predict::insight_age_gender_predict(insight_age_gender_predict_params params) :
    params_(std::make_unique<insight_age_gender_predict_params>(params))
{
    executor_ = mxnet_aux::create_executor(params.model_params_, params.model_symbols_, params.context_, params.shape_);
    image_vector_.resize(params.shape_.Size());
}

insight_age_gender_predict::~insight_age_gender_predict()
{

}

insight_age_gender_info insight_age_gender_predict::forward(const dlib::matrix<dlib::rgb_pixel> &input)
{
    std::vector<dlib::matrix<dlib::rgb_pixel> const*> faces;
    faces.emplace_back(&input);
    img_cvt::dlib_matrix_to_separate_rgb_plane(faces, image_vector_);
    return forward(image_vector_, 1)[0];
}

std::vector<insight_age_gender_info> insight_age_gender_predict::
forward(const std::vector<dlib::matrix<dlib::rgb_pixel> > &input)
{
    if(input.empty()){
        return {};
    }

    using dlib_const_images_ptr = std::vector<dlib::matrix<dlib::rgb_pixel> const*>;
    auto const forward_count = static_cast<size_t>(std::ceil(input.size() / static_cast<float>(params_->shape_[0])));
    std::vector<insight_age_gender_info> result;
    for(size_t i = 0, index = 0; i != forward_count; ++i){
        dlib_const_images_ptr faces;
        for(size_t j = 0; j != params_->shape_[0] && index < input.size(); ++j){
            faces.emplace_back(&input[index++]);
        }
        img_cvt::dlib_matrix_to_separate_rgb_plane(faces, image_vector_);
        auto features = forward(image_vector_, static_cast<size_t>(faces.size()));
        std::move(std::begin(features), std::end(features), std::back_inserter(result));
    }

    return result;
}

std::vector<insight_age_gender_info> insight_age_gender_predict::
forward(const std::vector<float> &input, size_t batch_size)
{
    executor_->arg_dict()["data"].SyncCopyFromCPU(input.data(), input.size());
    if(params_->shape_[0] > 1){
        executor_->arg_dict()["data1"] = batch_size;
    }
    executor_->Forward(false);
    std::vector<insight_age_gender_info> result;
    if(!executor_->outputs.empty()){
        auto features = executor_->outputs[0].Copy(Context(kCPU, 0));        
        int constexpr features_size = 202;
        features.WaitToRead();
        for(size_t i = 0; i != batch_size; ++i){
            auto const *ptr = features.GetData() + i * features_size;
            insight_age_gender_info info;
            info.gender_ = ptr[0] > ptr[1] ? gender_info::female_ : gender_info::male_;
            for(int i = 2; i < features_size; i += 2){
                if(ptr[i + 1] > ptr[i]){
                    info.age_ += 1;
                }
            }
            result.emplace_back(info);
        }
        return result;
    }

    return result;
}

}

}
