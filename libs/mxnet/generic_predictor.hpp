#ifndef GENERIC_PREDICT_FUNCTIONS_HPP
#define GENERIC_PREDICT_FUNCTIONS_HPP

#include "common.hpp"
#include "../image_format_convert/dlib_to_array.hpp"

#include <mxnet-cpp/MxNetCpp.h>
#include <vector>

namespace ocv{

namespace mxnet_aux{

template<typename Return, typename Functor>
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
    Functor func_;
    std::vector<float> image_vector_;
    size_t max_batch_size_;
};

template<typename Return, typename Functor>
generic_predictor<Return, Functor>::generic_predictor(const std::string &model_params,
                                                      const std::string &model_symbol,
                                                      const mxnet::cpp::Context &context,
                                                      const mxnet::cpp::Shape &shape)
{
    executor_ = create_executor(model_params, model_symbol, context, shape);
    image_vector_.resize(shape.Size());
    max_batch_size_ = shape[0];
}

template<typename Return, typename Functor>
generic_predictor<Return, Functor>::~generic_predictor(){}

template<typename Return, typename Functor>
Return generic_predictor<Return, Functor>::forward(const dlib::matrix<dlib::rgb_pixel> &input)
{
    std::vector<dlib::matrix<dlib::rgb_pixel> const*> faces;
    faces.emplace_back(&input);
    img_cvt::dlib_matrix_to_separate_rgb_plane(faces, image_vector_);
    return forward(1)[0];
}

template<typename Return, typename Functor>
std::vector<Return> generic_predictor<Return, Functor>::
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
        img_cvt::dlib_matrix_to_separate_rgb_plane(faces, image_vector_);

        auto features = forward(static_cast<size_t>(faces.size()));
        std::move(std::begin(features), std::end(features), std::back_inserter(result));
    }

    return result;
}

template<typename Return, typename Functor>
std::vector<Return> generic_predictor<Return, Functor>::forward(size_t batch_size)
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
        return func_(features, batch_size);
    }

    return {};
}

}

}

#endif // GENERIC_PREDICT_FUNCTIONS_HPP
