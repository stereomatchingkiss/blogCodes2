#ifndef IMAGEALGORITHM_HPP
#define IMAGEALGORITHM_HPP

#include <array>
#include <initializer_list>
#include <vector>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

namespace OCV
{

void calc_back_project(std::initializer_list<cv::Mat> images, std::initializer_list<int> channels, cv::InputArray hist, cv::OutputArray backProject,
                       std::initializer_list<float[2]> ranges, double scale = 1, bool uniform = true);

inline
cv::Mat calc_back_project(std::initializer_list<cv::Mat> images, std::initializer_list<int> channels, cv::InputArray hist,
                       std::initializer_list<float[2]> ranges, double scale = 1, bool uniform = true)
{
    cv::Mat output;
    calc_back_project(images, channels, hist, output, ranges, scale, uniform);

    return output;
}

template<size_t N>
void calc_back_project(std::initializer_list<cv::Mat> images, std::initializer_list<int> channels, cv::InputArray hist, cv::OutputArray backProject,
                       std::array<float[2], N> ranges, double scale = 1, bool uniform = true)
{
    std::array<float const*, N> d_ranges;
    for(size_t i = 0; i != N; ++i){
        d_ranges[i] = ranges[i];
    }

    cv::calcBackProject(std::begin(images), images.size(), std::begin(channels), hist, backProject, &d_ranges[0], scale, uniform);
}

template<size_t N>
inline
cv::Mat calc_back_project(std::initializer_list<cv::Mat> images, std::initializer_list<int> channels, cv::InputArray hist,
                          std::array<float[2], N> ranges, double scale = 1, bool uniform = true)
{
    cv::Mat output;
    calc_back_project<N>(images, channels, hist, output, ranges, scale, uniform);

    return output;
}

/**
 * @brief encapsulation of cv::caclHist, all of the meaning of the parameters are same as the cv::caclHist, but these api
 * are easier to use. ex : cv::Mat hist = calc_histogram({hsv}, {0, 1}, {180, 256}, {{ {0, 180}, {0, 256} }});
 */
void calc_histogram(std::initializer_list<cv::Mat> images, cv::OutputArray &output, std::initializer_list<int> channels,
                    std::initializer_list<int> hist_sizes, std::initializer_list<float[2]> ranges,
                    cv::InputArray const &mask = cv::Mat(), bool uniform = true,
                    bool accumulate = false);

inline
cv::MatND calc_histogram(std::initializer_list<cv::Mat> images, std::initializer_list<int> channels,
                         std::initializer_list<int> hist_sizes, std::initializer_list<float[2]> ranges,
                         cv::InputArray const &mask = cv::Mat(), bool uniform = true,
                         bool accumulate = false)
{
    cv::MatND output;
    calc_histogram(images, output, channels, hist_sizes, ranges, mask, uniform, accumulate);

    return output;
}

template<size_t N>
void calc_histogram(std::initializer_list<cv::Mat> images, cv::OutputArray &output, std::initializer_list<int> channels,
                    std::initializer_list<int> hist_sizes, std::array<float[2], N> const &ranges,
                    cv::InputArray const &mask = cv::Mat(), bool uniform = true,
                    bool accumulate = false)
{
    std::array<float const*, N> d_ranges;
    for(size_t i = 0; i != N; ++i){
        d_ranges[i] = ranges[i];
    }

    cv::calcHist(std::begin(images), images.size(), std::begin(channels), mask, output, channels.size(), std::begin(hist_sizes), &d_ranges[0], uniform ,accumulate);
}

template<size_t N>
inline
cv::MatND calc_histogram(std::initializer_list<cv::Mat> images, std::initializer_list<int> channels,
                         std::initializer_list<int> hist_sizes, std::array<float[2], N> const &ranges,
                         cv::InputArray const &mask = cv::Mat(), bool uniform = true,
                         bool accumulate = false)
{
    cv::MatND output;
    calc_histogram<N>(images, output, channels, hist_sizes, ranges, mask, uniform, accumulate);

    return output;
}

inline void mix_channels(cv::Mat const &src, cv::Mat &dst, std::initializer_list<int> from_to)
{
    cv::mixChannels(&src, 1, &dst, 1, std::begin(from_to), from_to.size() / 2);
}

}

#endif // IMAGEALGORITHM_HPP
