#ifndef FACE_DETECTOR_NET_HPP
#define FACE_DETECTOR_NET_HPP

#include <dlib/dnn.h>

namespace ocv
{

namespace face{

namespace dlib_net{

using namespace dlib;

template <long num_filters, typename SUBNET> using con5d = con<num_filters,5,5,2,2,SUBNET>;
template <long num_filters, typename SUBNET> using con5  = con<num_filters,5,5,1,1,SUBNET>;

template <typename SUBNET> using downsampler =
relu<affine<con5d<32, relu<affine<con5d<32, relu<affine<con5d<16,SUBNET>>>>>>>>>;

template <typename SUBNET> using rcon5  = relu<affine<con5<45,SUBNET>>>;

using face_detect_net =
loss_mmod<con<1,9,9,1,1,rcon5<rcon5<rcon5<downsampler<input_rgb_image_pyramid<pyramid_down<6>>>>>>>>;

}

}

}

#endif // FACE_DETECTOR_NET_HPP
