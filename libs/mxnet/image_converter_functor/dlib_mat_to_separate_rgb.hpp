#ifndef DLIB_MAT_TO_SEPARATE_RGB_HPP
#define DLIB_MAT_TO_SEPARATE_RGB_HPP

#include "../../image_format_convert/dlib_to_array.hpp"

namespace ocv{

namespace mxnet_aux{

class dlib_mat_to_separate_rgb
{
public:
    template<typename T, typename U>
    void operator()(std::vector<dlib::matrix<T> const*> inputs, U &output) const
    {
        size_t index = 0;
        for(size_t i = 0; i != inputs.size(); ++i){
            dlib_matrix_to_separate_rgb_plane(inputs[i], index, output);
        }
    }
};

}

}

#endif // DLIB_MAT_TO_SEPARATE_RGB_HPP
