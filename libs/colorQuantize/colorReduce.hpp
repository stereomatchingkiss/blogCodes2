#ifndef COLORREDUCE_HPP
#define COLORREDUCE_HPP

namespace cv
{
  class Mat;
}

namespace OCV
{

void color_reduce(cv::Mat &input, cv::Mat &output, size_t div = 64);

}

#endif // COLORREDUCE_HPP
