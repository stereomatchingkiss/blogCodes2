#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <ocv_libs/core/for_each.hpp>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

#include <string>
#include <vector>

template<typename Img>
Img cvmat_to_img(cv::Mat const& input,
                 float_t scale_min = -1,
                 float_t scale_max = 1)
{
    CV_Assert(input.type() == CV_8U || input.type() == CV_8UC3);
    CV_Assert(scale_min < scale_max);

    Img img;
    ocv::for_each<uchar>(input, [&](uchar c)
    {
       img.emplace_back(scale_min + (scale_max - scale_min) * c / 255.0);
    });

    return img;
}

#endif // UTILITY_HPP
