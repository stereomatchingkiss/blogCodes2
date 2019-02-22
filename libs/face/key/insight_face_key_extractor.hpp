#ifndef OCV_FACE_INSIGHT_FACE_KEY_EXTRACTOR_HPP
#define OCV_FACE_INSIGHT_FACE_KEY_EXTRACTOR_HPP

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

namespace mxnet_aux{

class generic_predictor;

}

namespace face{

class insight_face_key;
struct insight_face_key_extractor_params;

class insight_face_key_extractor
{
public:
    explicit insight_face_key_extractor(insight_face_key_extractor_params const &params);
    ~insight_face_key_extractor();

    insight_face_key forward(dlib::matrix<dlib::rgb_pixel> const &input);
    std::vector<insight_face_key> forward(std::vector<dlib::matrix<dlib::rgb_pixel>> const &input);

private:
    using dlib_const_images_ptr = std::vector<dlib::matrix<dlib::rgb_pixel> const*>;

    std::vector<insight_face_key> extract_key(mxnet::cpp::NDArray const &features, size_t batch_size) const;

    std::unique_ptr<ocv::mxnet_aux::generic_predictor> predictor_;
};

}

}

#endif // OCV_FACE_INSIGHT_FACE_KEY_EXTRACTOR_HPP
