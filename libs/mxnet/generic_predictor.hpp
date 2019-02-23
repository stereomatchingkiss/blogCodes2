#ifndef GENERIC_PREDICT_FUNCTIONS_HPP
#define GENERIC_PREDICT_FUNCTIONS_HPP

#include "common.hpp"
#include "image_converter_functor/dlib_mat_to_separate_rgb.hpp"

#include <mxnet-cpp/MxNetCpp.h>
#include <vector>

namespace ocv{

namespace mxnet_aux{

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

    Return forward(const dlib::matrix<dlib::rgb_pixel> &input);

    std::vector<Return> forward(const std::vector<dlib::matrix<dlib::rgb_pixel>> &input);

private:    
    std::vector<Return> forward(size_t batch_size);

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
Return generic_predictor<Return, ProcessFeature, ImageConvert>::forward(const dlib::matrix<dlib::rgb_pixel> &input)
{
    std::vector<dlib::matrix<dlib::rgb_pixel> const*> faces;
    faces.emplace_back(&input);
    img_convert_func_(faces, image_vector_);
    return forward(1)[0];
}

template<typename Return, typename ProcessFeature, typename ImageConvert>
std::vector<Return> generic_predictor<Return, ProcessFeature, ImageConvert>::
forward(const std::vector<dlib::matrix<dlib::rgb_pixel> > &input)
{
    if(input.empty()){
        return {};
    }

    using dlib_const_images_ptr = std::vector<dlib::matrix<dlib::rgb_pixel> const*>;
    auto const forward_count = static_cast<size_t>(std::ceil(input.size() / static_cast<float>(max_batch_size_)));
    std::vector<Return> result;
    for(size_t i = 0, index = 0; i != forward_count; ++i){
        std::vector<dlib::matrix<dlib::rgb_pixel> const*> faces;
        for(size_t j = 0; j != max_batch_size_ && index < input.size(); ++j){
            faces.emplace_back(&input[index++]);
        }
        img_convert_func_(faces, image_vector_);

        auto features = forward(static_cast<size_t>(faces.size()));
        std::move(std::begin(features), std::end(features), std::back_inserter(result));
    }

    return result;
}

template<typename Return, typename ProcessFeature, typename ImageConvert>
std::vector<Return> generic_predictor<Return, ProcessFeature, ImageConvert>::forward(size_t batch_size)
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

#endif // GENERIC_PREDICT_FUNCTIONS_HPP
