#ifndef FACE_KEY_EXTRACTOR_HPP
#define FACE_KEY_EXTRACTOR_HPP

#include <opencv2/core.hpp>

#include <dlib/image_processing.h>

#include <memory>
#include <string>

namespace mxnet{

namespace cpp{

class Context;
class Executor;
class NDArray;

}

}

namespace mxnet_tool{

class face_key;
struct face_key_extractor_params;

class face_key_extractor
{
public:
    explicit face_key_extractor(face_key_extractor_params const &params);
    ~face_key_extractor() = default;

    face_key forward(dlib::matrix<dlib::rgb_pixel> const &input);
    face_key forward(cv::Mat const &input);

private:
    face_key forward(mxnet::cpp::NDArray const &input);

    std::unique_ptr<mxnet::cpp::Context> context_;
    std::unique_ptr<mxnet::cpp::Executor> executor_;
};

}

#endif // FACE_KEY_EXTRACTOR_HPP
