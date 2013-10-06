#ifndef HISTOGRAM_HPP
#define HISTOGRAM_HPP

#include <array>
#include <initializer_list>
#include <vector>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

namespace OCV
{

/**
 * @brief encapsulation of cv::caclHist, all of the meaning of the parameters are same as the cv::caclHist, but these api
 * are easier to use. ex : cv::Mat hist = calc_histogram({hsv}, {0, 1}, {180, 256}, {{ {0, 180}, {0, 256} }});
 */
void calc_histogram(std::initializer_list<cv::Mat> images, cv::OutputArray output, std::initializer_list<int> channels,
                    std::initializer_list<int> hist_sizes, std::initializer_list<float[2]> ranges,
                    cv::InputArray mask = cv::Mat(), bool uniform = true,
                    bool accumulate = false);

/**
 * @brief encapsulation of cv::caclHist, all of the meaning of the parameters are same as the cv::caclHist, but these api
 * are easier to use. ex : cv::Mat hist = calc_histogram({hsv}, {0, 1}, {180, 256}, {{ {0, 180}, {0, 256} }});
 *
 * @return histogram
 */
inline
cv::MatND calc_histogram(std::initializer_list<cv::Mat> images, std::initializer_list<int> channels,
                         std::initializer_list<int> hist_sizes, std::initializer_list<float[2]> ranges,
                         cv::InputArray mask = cv::Mat(), bool uniform = true,
                         bool accumulate = false)
{
    cv::MatND output;
    calc_histogram(images, output, channels, hist_sizes, ranges, mask, uniform, accumulate);

    return output;
}

/**
 * @brief encapsulation of cv::caclHist, all of the meaning of the parameters are same as the cv::caclHist, but these api
 * are easier to use. ex : cv::Mat hist = calc_histogram({hsv}, {0, 1}, {180, 256}, {{ {0, 180}, {0, 256} }});This api
 * could reduce the times of memory allocation when compare to non-template version
 */
template<size_t N>
void calc_histogram(std::initializer_list<cv::Mat> images, cv::OutputArray output, std::initializer_list<int> channels,
                    std::initializer_list<int> hist_sizes, std::array<float[2], N> const &ranges,
                    cv::InputArray mask = cv::Mat(), bool uniform = true,
                    bool accumulate = false)
{
    std::array<float const*, N> d_ranges;
    for(size_t i = 0; i != N; ++i){
        d_ranges[i] = ranges[i];
    }

    cv::calcHist(std::begin(images), images.size(), std::begin(channels), mask, output, channels.size(), std::begin(hist_sizes), &d_ranges[0], uniform ,accumulate);
}

template<size_t N>
void calc_histogram(std::initializer_list<cv::Mat> images, cv::OutputArray output, std::initializer_list<int> channels,
                    std::initializer_list<int> hist_sizes, std::initializer_list<float[2]> ranges,
                    cv::InputArray mask = cv::Mat(), bool uniform = true,
                    bool accumulate = false)
{
    std::array<float const*, N> d_ranges;
    for(size_t i = 0; i != N; ++i){
        d_ranges[i] = *(std::begin(ranges) + i);
    }

    cv::calcHist(std::begin(images), images.size(), std::begin(channels), mask, output, channels.size(), std::begin(hist_sizes), &d_ranges[0], uniform ,accumulate);
}

/**
 * @brief encapsulation of cv::caclHist, all of the meaning of the parameters are same as the cv::caclHist, but these api
 * are easier to use. ex : cv::Mat hist = calc_histogram({hsv}, {0, 1}, {180, 256}, {{ {0, 180}, {0, 256} }});This api
 * could reduce the times of memory allocation when compare to non-template version
 *
 * @return histogram
 */
template<size_t N>
inline
cv::MatND calc_histogram(std::initializer_list<cv::Mat> images, std::initializer_list<int> channels,
                         std::initializer_list<int> hist_sizes, std::array<float[2], N> const &ranges,
                         cv::InputArray mask = cv::Mat(), bool uniform = true,
                         bool accumulate = false)
{
    cv::MatND output;
    calc_histogram<N>(images, output, channels, hist_sizes, ranges, mask, uniform, accumulate);

    return output;
}

template<size_t N>
inline
cv::MatND calc_histogram(std::initializer_list<cv::Mat> images, std::initializer_list<int> channels,
                         std::initializer_list<int> hist_sizes, std::initializer_list<float[2]> ranges,
                         cv::InputArray mask = cv::Mat(), bool uniform = true,
                         bool accumulate = false)
{
    cv::MatND output;
    calc_histogram<N>(images, output, channels, hist_sizes, ranges, mask, uniform, accumulate);

    return output;
}

}

#endif // HISTOGRAM_HPP
