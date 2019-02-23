#ifndef OCV_IMG_CVT_DLIB_MAT_TO_SEPARATE_RGB_HPP
#define OCV_IMG_CVT_DLIB_MAT_TO_SEPARATE_RGB_HPP


#include <dlib/image_processing.h>

#include <vector>

namespace ocv{

namespace img_cvt{

template<typename T, typename U>
void dlib_matrix_to_separate_rgb_plane(dlib::matrix<T> const *input, size_t &offset, U &output)
{
    for(size_t ch = 0; ch != 3; ++ch){
        for(long row = 0; row != input->nr(); ++row){
            for(long col = 0; col != input->nc(); ++col){
                auto const &pix = (*input)(row, col);
                switch(ch){
                case 0:
                    output[offset++] = pix.red;
                    break;
                case 1:
                    output[offset++] = pix.green;
                    break;
                case 2:
                    output[offset++] = pix.blue;
                    break;
                default:
                    break;
                }
            }
        }
    }
}

template<typename T, typename U>
void dlib_matrix_to_separate_rgb_plane(std::vector<dlib::matrix<T> const*> inputs, U &output)
{
    size_t index = 0;
    for(size_t i = 0; i != inputs.size(); ++i){
        dlib_matrix_to_separate_rgb_plane(inputs[i], index, output);
    }
}

template<typename T, typename U>
void dlib_matrix_to_rgb_plane(dlib::matrix<T> const *input, size_t &offset, U &output)
{
    for(long row = 0; row != input->nr(); ++row){
        for(long col = 0; col != input->nc(); ++col){
            auto const &pix = (*input)(row, col);
            for(size_t ch = 0; ch != 3; ++ch){
                switch(ch){
                case 0:
                    output[offset++] = pix.red;
                    break;
                case 1:
                    output[offset++] = pix.green;
                    break;
                case 2:
                    output[offset++] = pix.blue;
                    break;
                default:
                    break;
                }
            }
        }
    }
}

template<typename T, typename U>
void dlib_matrix_to_rgb_plane(std::vector<dlib::matrix<T> const*> inputs, U &output)
{
    size_t index = 0;
    for(size_t i = 0; i != inputs.size(); ++i){
        dlib_matrix_to_rgb_plane(inputs[i], index, output);
    }
}

}

}

#endif // OCV_IMG_CVT_DLIB_MAT_TO_SEPARATE_RGB_HPP
