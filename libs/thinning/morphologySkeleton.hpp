#ifndef MORPHOLOGYSKELETON_HPP
#define MORPHOLOGYSKELETON_HPP

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

namespace OCV
{

void morphology_skeleton(cv::Mat &input, cv::Mat &output, cv::Mat const &kernel = cv::getStructuringElement(cv::MORPH_CROSS, cv::Size(3, 3)));

}

#endif // MORPHOLOGYSKELETON_HPP
