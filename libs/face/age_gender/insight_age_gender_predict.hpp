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

class NDArray;

}

}

namespace ocv{

namespace mxnet_aux{

class generic_predictor;

}

namespace face{

struct insight_age_gender_info;
struct insight_age_gender_predict_params;

/**
 * Predict age and gender by the model of insight face
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
    std::vector<insight_age_gender_info> predict_age_gender(mxnet::cpp::NDArray const &features,
                                                            size_t batch_size) const;

    std::unique_ptr<ocv::mxnet_aux::generic_predictor> predictor_;
};

}

}

#endif // OCV_FACE_INSIGHT_AGE_GENDER_PREDICT_HPP
