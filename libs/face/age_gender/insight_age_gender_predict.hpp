#ifndef OCV_FACE_INSIGHT_AGE_GENDER_PREDICT_HPP
#define OCV_FACE_INSIGHT_AGE_GENDER_PREDICT_HPP

#include "insight_age_gender_info.hpp"

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

struct insight_age_gender_info;
struct insight_age_gender_predict_params;

/**
 * Predict age and gender by the model of insight face
 * @todo Reduce duplicate codes since the codes are almost same
 * as the "insight_face_key_extractor"
 */
class insight_age_gender_predict
{
public:
    explicit insight_age_gender_predict(insight_age_gender_predict_params params);
    ~insight_age_gender_predict();

    insight_age_gender_predict& operator=(insight_age_gender_predict &&) = default;
    insight_age_gender_predict(insight_age_gender_predict &&) = default;

    insight_age_gender_info forward(dlib::matrix<dlib::rgb_pixel> const &input);
    std::vector<insight_age_gender_info> forward(std::vector<dlib::matrix<dlib::rgb_pixel>> const &input);

private:
    std::vector<insight_age_gender_info> forward(std::vector<float> const &input, size_t batch_size);

    std::unique_ptr<mxnet::cpp::Executor> executor_;
    std::vector<float> image_vector_;
    std::unique_ptr<insight_age_gender_predict_params> params_;
};

}

}

#endif // OCV_FACE_INSIGHT_AGE_GENDER_PREDICT_HPP
