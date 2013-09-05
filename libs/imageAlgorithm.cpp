#include "imageAlgorithm.hpp"

namespace OCV
{

void calc_back_project(std::initializer_list<cv::Mat> images, std::initializer_list<int> channels, cv::InputArray hist,
                       cv::OutputArray backProject, std::initializer_list<float[2]> ranges, double scale, bool uniform )
{
    size_t const sizes = ranges.size();
    std::vector<float const*> d_ranges(sizes);
    for(size_t i = 0; i != sizes; ++i){
        d_ranges[i] = *(std::begin(ranges) + i);
    }

    cv::calcBackProject(std::begin(images), images.size(), std::begin(channels), hist, backProject, &d_ranges[0], scale, uniform);
}

void calc_histogram(std::initializer_list<cv::Mat> images, cv::OutputArray &output, std::initializer_list<int> channels,
                    std::initializer_list<int> hist_sizes, std::initializer_list<float[2]> ranges,
                    cv::InputArray const &mask, bool uniform, bool accumulate)
{    
    size_t const sizes = ranges.size();
    std::vector<float const*> d_ranges(sizes);
    for(size_t i = 0; i != sizes; ++i){
        d_ranges[i] = *(std::begin(ranges) + i);
    }

    cv::calcHist(std::begin(images), images.size(), std::begin(channels), mask, output,
                 channels.size(), std::begin(hist_sizes), &d_ranges[0], uniform ,accumulate);
}

}
