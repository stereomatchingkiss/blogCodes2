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

/*cv::Mat histProject::get_projection_map_hue(cv::Mat const &input, cv::Mat const &model)
{
    convert_to_hsv(input, input_hsv_);
    convert_to_hsv(model, model_hsv_);

    //cv::Mat probability_map = OCV::calc_back_project<1>({input_hsv_}, {0}, model_hist, {{ {0, 180} }});

    //return probability_map;


}

cv::Mat histProject::get_projection_map_hue_sat(cv::Mat const &input, cv::Mat const &model)
{

}

void histProject::convert_to_hsv(cv::Mat const &input, cv::Mat &dst)
{
    input.copyTo(dst);
    if(dst.channels() == 4){
        cv::cvtColor(dst, dst, CV_BGRA2BGR);
    }

    cv::cvtColor(dst, dst, CV_BGR2HSV);
}*/

}
