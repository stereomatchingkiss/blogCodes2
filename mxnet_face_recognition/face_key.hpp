#ifndef FACE_KEY_HPP
#define FACE_KEY_HPP

#include <mxnet-cpp/ndarray.h>

namespace mxnet_tool{

class face_key
{
public:    
    explicit face_key(mxnet::cpp::NDArray key = mxnet::cpp::NDArray());

    bool is_empty_key() const noexcept;
    /**
     * Compare similarity of face_key
     * @return value within [0, 1], bigger value mean more similar
     */
    double similarity(face_key const &input) const;

private:
    mxnet::cpp::NDArray key_;
};

}

#endif // FACE_KEY_HPP
