#ifndef OCV_FACE_INSIGHT_FACE_KEY_EXTRACTOR_HPP
#define OCV_FACE_INSIGHT_FACE_KEY_EXTRACTOR_HPP

#include "../../mxnet/generic_predictor.hpp"

#include "insight_face_key.hpp"

namespace ocv{

namespace face{

struct extract_face_key_functor
{
    std::vector<insight_face_key>
    operator()(const mxnet::cpp::NDArray &features, size_t batch_size) const;
};

using insight_face_key_extractor = mxnet_aux::generic_predictor<insight_face_key, extract_face_key_functor>;

}

}

#endif // OCV_FACE_INSIGHT_FACE_KEY_EXTRACTOR_HPP
