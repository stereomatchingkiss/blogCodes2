#ifndef OCV_FACE_INSIGHT_FACE_KEY_HPP
#define OCV_FACE_INSIGHT_FACE_KEY_HPP

#include <mxnet-cpp/ndarray.h>

namespace ocv{

namespace face{

class insight_face_key
{
public:    
    explicit insight_face_key(mxnet::cpp::NDArray key = mxnet::cpp::NDArray());

    bool is_empty_key() const noexcept;
    /**
     * Compare similarity of face_key
     * @return value within [-1, 1], bigger value mean more similar(it use cosine similarity
     * under the hood)
     */
    double similarity(insight_face_key const &input) const;

private:
    mxnet::cpp::NDArray key_;
};

}

}

#endif // OCV_FACE_INSIGHT_FACE_KEY_HPP
