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

    if (scale_min >= scale_max){
        throw std::runtime_error("scale_max must be greater than scale_min");
    }

    Img img;
    if (input.type() == CV_8UC3){
        int const w = input.cols;
        int const h = input.rows;
        int const area = w * h;
        float const range = scale_max - scale_min;
        img.resize(area * input.channels(), scale_min);        

        for (int c = 0; c < input.channels(); ++c) {
            for (int y = 0; y < input.rows; ++y) {
                for (int x = 0; x < input.cols; ++x) {
                    img[c * area + y * w + x]
                            = scale_min + range *
                            input.at<cv::Vec3b>(y, x)[c] / 255.0;
                }
            }
        }
    }else{
        ocv::for_each<uchar>(input, [&](uchar c)
        {
           img.emplace_back(scale_min + (scale_max - scale_min) * c / 255.0);
        });
    }

    return img;
}

#endif // UTILITY_HPP
