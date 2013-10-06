#include <algorithm>

#include "histBackProject.hpp"
#include "histogram.hpp"
#include "utility.hpp"

namespace OCV
{

/**
 * @brief calc_back_project : encapsulate cv::calcBackProject, provide friendly api, all of the parameters meaning are same as
 * the cv::calcBackProject
 */
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

/**
 * @brief easy class for histogram backprojection, this api only take one dimension histogram(hue) for back projection
 * @param input : the target image
 * @param roi : region of interest
 * @param min_saturation : min saturation, if > 0, the program will filter out the pixels lower than min saturation;else do nothing
 * @return probability map
 */
cv::Mat histProject::get_projection_map_hue(cv::Mat const &input, cv::Mat const &roi, int min_saturation)
{
    cv::Mat map;
    get_projection_map_hue(input, roi, map, min_saturation);

    return map;
}

/**
 * @brief easy class for histogram backprojection, this api only take one dimension histogram(hue) for back projection
 *        this api expect the roi already convert to the color space of hsv
 * @param input : the target image
 * @param roi : region of interest, should convert to hsv
 * @param min_saturation : min saturation, if > 0, the program will filter out the pixels lower than min saturation;else do nothing
 * @return probability map
 */
cv::Mat histProject::get_projection_map_hue_lazy(cv::Mat const &input, cv::Mat const &roi, int min_saturation)
{
    cv::Mat map;
    get_projection_map_hue_lazy(input, roi, map, min_saturation);

    return map;
}

/**
 * @brief easy class for histogram backprojection, this api only take one dimension histogram(hue) for back projection
 * @param input : the target image
 * @param roi : region of interest
 * @param output : probability map
 * @param min_saturation : min saturation, if > 0, the program will filter out the pixels lower than min saturation;else do nothing
 */
void histProject::get_projection_map_hue(cv::Mat const &input, cv::Mat const &roi, cv::Mat &output, int min_saturation)
{
    convert_to_hsv(roi, roi_hsv_);
    get_projection_map_hue_lazy(input, roi_hsv_, output, min_saturation);
}

/**
 * @brief easy class for histogram backprojection, this api only take one dimension histogram(hue) for back projection
 *        this api expect the roi already convert to the color space of hsv
 * @param input : the target image
 * @param roi : region of interest, should convert to hsv
 * @param output : probability map
 * @param min_saturation : min saturation, if > 0, the program will filter out the pixels lower than min saturation;else do nothing
 */
void histProject::get_projection_map_hue_lazy(cv::Mat const &input, cv::Mat const &roi, cv::Mat &output, int min_saturation)
{
    convert_to_hsv(input, input_hsv_);
    if(min_saturation > 0){
        filter_low_saturation_pixels(roi, model_saturation_mask_, min_saturation);
        calc_histogram<1>({roi}, roi_hist_, {0}, {180}, { {0, 180} }, model_saturation_mask_);

        filter_low_saturation_pixels(input_hsv_, map_saturation_mask_, min_saturation);
        OCV::calc_back_project<1>({input_hsv_}, {0}, roi_hist_, output, { {0, 180} });

        output &= map_saturation_mask_;
    }else{
        calc_histogram<1>({roi}, roi_hist_, {0}, {180}, { {0, 180} });
        OCV::calc_back_project<1>({input_hsv_}, {0}, roi_hist_, output, { {0, 180} });
    }
}

/**
 * @brief easy class for histogram backprojection, this api take two dimensions histogram(hue, saturation) for back projection
 * @param input : the target image
 * @param roi : region of interest
 * @param min_saturation : min saturation, if > 0, the program will filter out the pixels lower than min saturation;else do nothing
 * @return probability map
 */
cv::Mat histProject::get_projection_map_hue_sat(cv::Mat const &input, cv::Mat const &roi, int min_saturation)
{
    cv::Mat map;
    get_projection_map_hue_sat(input, roi, map, min_saturation);

    return map;
}

/**
 * @brief easy class for histogram backprojection, this api take two dimensions histogram(hue, saturation) for back projection
 *        this api expect the roi already convert to the color space of hsv
 * @param input : the target image
 * @param roi : region of interest, should convert to hsv
 * @param min_saturation : min saturation, if > 0, the program will filter out the pixels lower than min saturation;else do nothing
 * @return probability map
 */
cv::Mat histProject::get_projection_map_hue_sat_lazy(cv::Mat const &input, cv::Mat const &roi, int min_saturation)
{
    cv::Mat map;
    get_projection_map_hue_sat_lazy(input, roi, map, min_saturation);

    return map;
}

/**
 * @brief easy class for histogram backprojection, this api take two dimensions histogram(hue) for back projection
 *        this api expect the roi already convert to the color space of hsv
 * @param input : the target image
 * @param roi : region of interest, should convert to hsv
 * @param output : probability map
 * @param min_saturation : min saturation, if > 0, the program will filter out the pixels lower than min saturation;else do nothing
 */
void histProject::get_projection_map_hue_sat(cv::Mat const &input, cv::Mat const &roi, cv::Mat &output, int min_saturation)
{
    convert_to_hsv(roi, roi_hsv_);
    get_projection_map_hue_sat_lazy(input, roi_hsv_, output, min_saturation);
}

void histProject::get_projection_map_hue_sat_lazy(cv::Mat const &input, cv::Mat const &roi, cv::Mat &output, int min_saturation)
{
    convert_to_hsv(input, input_hsv_);
    if(min_saturation > 0){
        filter_low_saturation_pixels(roi_hsv_, model_saturation_mask_, min_saturation);
        calc_histogram<2>({roi}, roi_hist_, {0, 1}, {180, 256}, { {0, 180}, {0, 256} }, model_saturation_mask_);

        filter_low_saturation_pixels(input_hsv_, map_saturation_mask_, min_saturation);
        OCV::calc_back_project<2>({input_hsv_}, {0, 1}, roi_hist_, output, { {0, 180}, {0, 256} });

        output &= map_saturation_mask_;
    }else{
        calc_histogram<2>({roi}, roi_hist_, {0, 1}, {180, 256}, { {0, 180}, {0, 256} });
        OCV::calc_back_project<2>({input_hsv_}, {0, 1}, roi_hist_, output, { {0, 180}, {0, 256} });
    }
}

/**
 * @brief convert input(bgr) to hsv color space
 * @param input  : bgra or bgr
 * @param output : hsv image
 */
void histProject::convert_to_hsv(cv::Mat const &input, cv::Mat &output) const
{
    input.copyTo(output);
    if(output.channels() == 4){
        cv::cvtColor(output, output, CV_BGRA2BGR);
    }

    cv::cvtColor(output, output, CV_BGR2HSV);
}

/**
 * @brief create mask which filter out low saturation pixels of input
 * @param input : input image, color-space should be hsv
 * @param output : mask after filter
 * @param min_saturation : min saturation, if > 0, the program will filter out the pixels lower than min saturation;else do nothing
 */
void histProject::filter_low_saturation_pixels(cv::Mat const &input, cv::Mat &output, int min_saturation) const
{
    output.create(input.size(), input.depth());
    mix_channels(input, output, {1, 0});
    cv::threshold(output, output, min_saturation, 255, cv::THRESH_BINARY);
}

}
