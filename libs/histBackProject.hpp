#ifndef HISTBACKPROJECT_HPP
#define HISTBACKPROJECT_HPP

#include <array>
#include <initializer_list>
#include <vector>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

namespace OCV
{

/**
 * @brief encapsulation of cv::calcBackProject, all of the meaning of the parameters are same as the cv::caclHist, but these api
 * are easier to use.
 * ex : calc_back_project({hsv}, {0, 1}, hist, map, { {0, 180}, {0, 256} });
 */
void calc_back_project(std::initializer_list<cv::Mat> images, std::initializer_list<int> channels, cv::InputArray hist, cv::OutputArray backProject,
                       std::initializer_list<float[2]> ranges, double scale = 1, bool uniform = true);

/**
 * @brief encapsulation of cv::calcBackProject, all of the meaning of the parameters are same as the cv::caclHist, but these api
 * are easier to use.
 * ex : cv::Mat hist = calc_back_project({hsv}, {0, 1}, hist, { {0, 180}, {0, 256} });
 */
inline
cv::Mat calc_back_project(std::initializer_list<cv::Mat> images, std::initializer_list<int> channels, cv::InputArray hist,
                       std::initializer_list<float[2]> ranges, double scale = 1, bool uniform = true)
{
    cv::Mat output;
    calc_back_project(images, channels, hist, output, ranges, scale, uniform);

    return output;
}

/**
 * @brief encapsulation of cv::calcBackProject, all of the meaning of the parameters are same as the cv::caclHist, but these api
 * are easier to use.This api could reduce the times of memory allocation when compare to non-template version
 * ex : calc_back_project({hsv}, {0, 1}, hist, map, {{ {0, 180}, {0, 256} }});
 */
template<size_t N>
void calc_back_project(std::initializer_list<cv::Mat> images, std::initializer_list<int> channels, cv::InputArray hist, cv::OutputArray backProject,
                       std::array<float[2], N> const &ranges, double scale = 1, bool uniform = true)
{
    std::array<float const*, N> d_ranges;
    for(size_t i = 0; i != N; ++i){
        d_ranges[i] = ranges[i];
    }

    cv::calcBackProject(std::begin(images), images.size(), std::begin(channels), hist, backProject, &d_ranges[0], scale, uniform);
}

template<size_t N>
void calc_back_project(std::initializer_list<cv::Mat> images, std::initializer_list<int> channels, cv::InputArray hist, cv::OutputArray backProject,
                       std::initializer_list<float[2]> ranges, double scale = 1, bool uniform = true)
{
    std::array<float const*, N> d_ranges;
    for(size_t i = 0; i != N; ++i){
        d_ranges[i] = *(std::begin(ranges) + i);
    }

    cv::calcBackProject(std::begin(images), images.size(), std::begin(channels), hist, backProject, &d_ranges[0], scale, uniform);
}

/**
 * @brief encapsulation of cv::calcBackProject, all of the meaning of the parameters are same as the cv::caclHist, but these api
 * are easier to use.This api could reduce the times of memory allocation when compare to non-template version
 * ex : cv::Mat hist = calc_back_project({hsv}, {0, 1}, hist, {{ {0, 180}, {0, 256} }});
 */
template<size_t N>
inline
cv::Mat calc_back_project(std::initializer_list<cv::Mat> images, std::initializer_list<int> channels, cv::InputArray hist,
                          std::array<float[2], N> const &ranges, double scale = 1, bool uniform = true)
{
    cv::Mat output;
    calc_back_project<N>(images, channels, hist, output, ranges, scale, uniform);

    return output;
}

template<size_t N>
inline
cv::Mat calc_back_project(std::initializer_list<cv::Mat> images, std::initializer_list<int> channels, cv::InputArray hist,
                          std::initializer_list<float[2]> ranges, double scale = 1, bool uniform = true)
{
    cv::Mat output;
    calc_back_project<N>(images, channels, hist, output, ranges, scale, uniform);

    return output;
}

class histProject
{
public:
    histProject() = default;
    histProject(histProject const&) = delete;
    histProject& operator=(histProject const&) = delete;

    cv::Mat get_projection_map_hue(cv::Mat const &input, cv::Mat const &roi, int min_saturation = 65);
    cv::Mat get_projection_map_hue_lazy(cv::Mat const &input, cv::Mat const &roi, int min_saturation = 65);
    void get_projection_map_hue(cv::Mat const &input, cv::Mat const &roi, cv::Mat &output, int min_saturation = 65);
    void get_projection_map_hue_lazy(cv::Mat const &input, cv::Mat const &roi, cv::Mat &output, int min_saturation = 65);

    cv::Mat get_projection_map_hue_sat(cv::Mat const &input, cv::Mat const &roi, int min_saturation = 65);
    cv::Mat get_projection_map_hue_sat_lazy(cv::Mat const &input, cv::Mat const &roi, int min_saturation = 65);
    void get_projection_map_hue_sat(cv::Mat const &input, cv::Mat const &roi, cv::Mat &output, int min_saturation = 65);
    void get_projection_map_hue_sat_lazy(cv::Mat const &input, cv::Mat const &roi, cv::Mat &output, int min_saturation = 65);

private:
    void convert_to_hsv(cv::Mat const &input, cv::Mat &output) const;
    void filter_low_saturation_pixels(cv::Mat const &input, cv::Mat &output, int min_saturation) const;

private:
    cv::Mat input_hsv_;
    cv::Mat map_saturation_mask_;
    cv::Mat model_saturation_mask_; //temporary saturation for filter out low saturation pixels of the model_hsv_
    cv::Mat roi_hist_;
    cv::Mat roi_hsv_;
};

}

#endif // HISTBACKPROJECT_HPP
