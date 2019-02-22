#ifndef GENERIC_PREDICT_FUNCTIONS_HPP
#define GENERIC_PREDICT_FUNCTIONS_HPP

#include <dlib/image_processing.h>

#include <mxnet-cpp/ndarray.h>

#include <functional>
#include <vector>

namespace mxnet{

namespace cpp{

class Context;
class Executor;

}

}

namespace ocv{

namespace mxnet_aux{

class generic_predictor
{
public:
    generic_predictor(std::string const &model_params,
                      std::string const &model_symbol,
                      mxnet::cpp::Context const &context,
                      mxnet::cpp::Shape const &shape);
    ~generic_predictor();

    template<typename T, typename Functor>
    T forward(const dlib::matrix<dlib::rgb_pixel> &input, Functor func)
    {
        std::vector<dlib::matrix<dlib::rgb_pixel> const*> faces;
        faces.emplace_back(&input);
        img_cvt::dlib_matrix_to_separate_rgb_plane(faces, image_vector_);
        return forward<T>(1, func)[0];
    }

    template<typename T, typename Functor>
    void forward(const std::vector<dlib::matrix<dlib::rgb_pixel>> &input,
                 Functor func,
                 std::vector<T> &output)
    {
        if(input.empty()){
            return;
        }

        using dlib_const_images_ptr = std::vector<dlib::matrix<dlib::rgb_pixel> const*>;
        auto const forward_count = static_cast<size_t>(std::ceil(input.size() / static_cast<float>(max_batch_size_)));
        for(size_t i = 0, index = 0; i != forward_count; ++i){
            std::vector<dlib::matrix<dlib::rgb_pixel> const*> faces;
            for(size_t j = 0; j != max_batch_size_ && index < input.size(); ++j){
                faces.emplace_back(&input[index++]);
            }
            img_cvt::dlib_matrix_to_separate_rgb_plane(faces, image_vector_);

            auto features = forward<T>(static_cast<size_t>(faces.size()), func);
            std::move(std::begin(features), std::end(features), std::back_inserter(output));
        }
    }

private:
    template<typename T, typename Functor>
    std::vector<T> forward(size_t batch_size, Functor func)
    {
        executor_->arg_dict()["data"].SyncCopyFromCPU(image_vector_.data(), image_vector_.size());
        if(max_batch_size_ > 1){
            executor_->arg_dict()["data1"] = batch_size;
        }
        executor_->Forward(false);
        if(!executor_->outputs.empty()){
            auto features = executor_->outputs[0].Copy(Context(kCPU, 0));
            features.WaitToRead();
            return func(features, batch_size);
        }

        return {};
    }

    std::unique_ptr<mxnet::cpp::Executor> executor_;
    std::vector<float> image_vector_;
    size_t max_batch_size_;
};

}

}

#endif // GENERIC_PREDICT_FUNCTIONS_HPP
