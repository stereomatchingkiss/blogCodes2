#ifndef HARRISCORNERIMPL_HPP
#define HARRISCORNERIMPL_HPP

namespace cv
{
  class Mat;
}

void harris_corners(cv::Mat const &input, cv::Mat &output, int blockSize = 3, int ksize = 3, float k = 0.04);

#endif // HARRISCORNERIMPL_HPP
