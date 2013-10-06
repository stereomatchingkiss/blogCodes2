#include "histogram.hpp"

namespace OCV
{

/**
 * @brief calc_back_project : encapsulate cv::calcHist, provide friendly api, all of the parameters meaning are same as
 * the cv::calcHist
 */
void calc_histogram(std::initializer_list<cv::Mat> images, cv::OutputArray output, std::initializer_list<int> channels,
                    std::initializer_list<int> hist_sizes, std::initializer_list<float[2]> ranges,
cv::InputArray mask, bool uniform, bool accumulate)
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
