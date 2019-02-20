#ifndef FACE_KEY_EXTRACTOR_HPP
#define FACE_KEY_EXTRACTOR_HPP

#include <opencv2/core.hpp>

#include <dlib/image_processing.h>

#include <memory>
#include <string>
#include <vector>

namespace mxnet{

namespace cpp{

class Context;
class Executor;
class NDArray;

}

}

namespace ocv{

namespace face{

class face_key;
struct face_key_extractor_params;

class face_key_extractor
{
public:
    explicit face_key_extractor(face_key_extractor_params const &params);
    ~face_key_extractor() = default;

    face_key forward(dlib::matrix<dlib::rgb_pixel> const &input);
    std::vector<face_key> forward(std::vector<dlib::matrix<dlib::rgb_pixel>> const &input);

private:
    using dlib_const_images_ptr = std::vector<dlib::matrix<dlib::rgb_pixel> const*>;    
    std::vector<face_key> forward(std::vector<float> const &input, size_t batch_size);

    std::unique_ptr<mxnet::cpp::Executor> executor_;
    std::vector<float> image_vector_;
    std::unique_ptr<face_key_extractor_params> params_;
};

}

}

#endif // FACE_KEY_EXTRACTOR_HPP
