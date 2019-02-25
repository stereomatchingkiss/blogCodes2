#ifndef OCV_MXNET_AUX_GENERIC_PREDICT_FUNCTIONS_HPP
#define OCV_MXNET_AUX_GENERIC_PREDICT_FUNCTIONS_HPP

#include "common.hpp"
#include "image_converter_functor/dlib_mat_to_separate_rgb.hpp"

#include <mxnet-cpp/MxNetCpp.h>
#include <vector>

namespace ocv{

namespace mxnet_aux{

/**
 *A generic class for mxnet cnn predictor
 *@param Return Return type of the forward function
 *@param ProcessFeature The binary functor to process the features extracted
 *by the Forward function of the Executor.
 *@param ImageConvert Binary functor to convert the images to std::vector<float>
 *@code
 *struct predict_age_gender_functor
 *{
 *   std::vector<insight_age_gender_info>
 *   operator()(const mxnet::cpp::NDArray &features, size_t batch_size) const
 *   {
 *       std::vector<insight_age_gender_info> result;
 *       int constexpr features_size = 202;
 *       for(size_t i = 0; i != batch_size; ++i){
 *           auto const *ptr = features.GetData() + i * features_size;
 *           insight_age_gender_info info;
 *           info.gender_ = ptr[0] > ptr[1] ? gender_info::female_ : gender_info::male_;
 *           for(int i = 2; i < features_size; i += 2){
 *               if(ptr[i + 1] > ptr[i]){
 *                   info.age_ += 1;
 *               }
 *           }
 *           result.emplace_back(info);
 *       }
 *   return result;
 *   }
 *};
 *
 *using insight_age_gender_predict = mxnet_aux::generic_predictor<insight_age_gender_info, predict_age_gender_functor>;
 *@endcode
 */
template<typename Return, typename ProcessFeature, typename ImageConvert = dlib_mat_to_separate_rgb>
class generic_predictor
{
public:
    generic_predictor(std::string const &model_params,
                      std::string const &model_symbol,
                      mxnet::cpp::Context const &context,
                      mxnet::cpp::Shape const &shape);

    ~generic_predictor();

    generic_predictor& operator=(generic_predictor &&) = default;
    generic_predictor(generic_predictor &&) = default;

    template<typename T>
    Return forward(T const &input);

    template<typename T>
    std::vector<Return> forward(const std::vector<T> &input);

private:    
    std::vector<Return> forward_and_process_feature(size_t batch_size);

    std::unique_ptr<mxnet::cpp::Executor> executor_;
    std::vector<float> image_vector_;
    ImageConvert img_convert_func_;
    size_t max_batch_size_;
    ProcessFeature process_func_;
};

template<typename Return, typename ProcessFeature, typename ImageConvert>
generic_predictor<Return, ProcessFeature, ImageConvert>::generic_predictor(const std::string &model_params,
                                                                           const std::string &model_symbol,
                                                                           const mxnet::cpp::Context &context,
                                                                           const mxnet::cpp::Shape &shape)
{
    executor_ = create_executor(model_params, model_symbol, context, shape);
    image_vector_.resize(shape.Size());
    max_batch_size_ = shape[0];
}

template<typename Return, typename ProcessFeature, typename ImageConvert>
generic_predictor<Return, ProcessFeature, ImageConvert>::~generic_predictor(){}

template<typename Return, typename ProcessFeature, typename ImageConvert>
template<typename T>
Return generic_predictor<Return, ProcessFeature, ImageConvert>::forward(T const &input)
{
    std::vector<T const*> faces;
    faces.emplace_back(&input);
    img_convert_func_(faces, image_vector_);
    return forward_and_process_feature(1)[0];
}

template<typename Return, typename ProcessFeature, typename ImageConvert>
template<typename T>
std::vector<Return> generic_predictor<Return, ProcessFeature, ImageConvert>::
forward(const std::vector<T> &input)
{
    if(input.empty()){
        return {};
    }

    using dlib_const_images_ptr = std::vector<T const*>;
    auto const forward_count = static_cast<size_t>(std::ceil(input.size() / static_cast<float>(max_batch_size_)));
    std::vector<Return> result;
    for(size_t i = 0, index = 0; i != forward_count; ++i){
        std::vector<dlib::matrix<dlib::rgb_pixel> const*> faces;
        for(size_t j = 0; j != max_batch_size_ && index < input.size(); ++j){
            faces.emplace_back(&input[index++]);
        }
        img_convert_func_(faces, image_vector_);

        auto features = forward_and_process_feature(static_cast<size_t>(faces.size()));
        std::move(std::begin(features), std::end(features), std::back_inserter(result));
    }

    return result;
}

template<typename Return, typename ProcessFeature, typename ImageConvert>
std::vector<Return> generic_predictor<Return, ProcessFeature, ImageConvert>::
forward_and_process_feature(size_t batch_size)
{
    using namespace mxnet::cpp;
    executor_->arg_dict()["data"].SyncCopyFromCPU(image_vector_.data(), image_vector_.size());
    if(max_batch_size_ > 1){
        executor_->arg_dict()["data1"] = batch_size;
    }
    executor_->Forward(false);
    if(!executor_->outputs.empty()){
        auto features = executor_->outputs[0].Copy(Context(kCPU, 0));
        features.WaitToRead();
        return process_func_(features, batch_size);
    }

    return {};
}

}

}

#endif // OCV_MXNET_AUX_GENERIC_PREDICT_FUNCTIONS_HPP
