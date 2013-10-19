#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "colorReduce.hpp"

namespace OCV
{

void color_reduce(cv::Mat &input, cv::Mat &output, size_t div)
{
    if(input.data != output.data){
        output.create(input.size(), input.type());
    }

    uchar buffer[256];
    for(size_t i = 0; i != 256; ++i){
        buffer[i] = i / div * div + div / 2;
    }
    cv::Mat table(1, 256, CV_8U, buffer, sizeof(buffer));
    cv::LUT(input, table, output);
}

}
