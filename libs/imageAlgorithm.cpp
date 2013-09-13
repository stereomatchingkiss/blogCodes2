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

cv::Mat histProject::get_projection_map_hue(cv::Mat const &input, cv::Mat const &model, int min_saturation)
{
    cv::Mat map;
    get_projection_map_hue(input, model, map, min_saturation);

    return map;
}

void histProject::get_projection_map_hue(cv::Mat const &input, cv::Mat const &model, cv::Mat &output, int min_saturation)
{
    convert_to_hsv(input, input_hsv_);
    convert_to_hsv(model, model_hsv_);

    if(min_saturation > 0){
        model_saturation_mask_.create(model_hsv_.size(), model_hsv_.depth());
        mix_channels(model_hsv_, model_saturation_mask_, {1, 0});
        cv::threshold(model_saturation_mask_, model_saturation_mask_, min_saturation, 255, cv::THRESH_BINARY);
        calc_histogram<1>({model_hsv_}, model_hist_, {0}, {180}, {{ {0, 180} }}, model_saturation_mask_);

        map_saturation_mask_.create(input_hsv_.size(), input_hsv_.depth());
        mix_channels(input_hsv_, map_saturation_mask_, {1, 0});
        OCV::calc_back_project<1>({input_hsv_}, {0}, model_hist_, output, {{ {0, 180} }});
        output &= map_saturation_mask_;
    }else{
        calc_histogram<1>({model_hsv_}, model_hist_, {0}, {180}, {{ {0, 180} }});
        OCV::calc_back_project<1>({input_hsv_}, {0}, model_hist_, output, {{ {0, 180} }});
    }
}

cv::Mat histProject::get_projection_map_hue_sat(cv::Mat const &input, cv::Mat const &model, int min_saturation)
{
    cv::Mat map;
    get_projection_map_hue_sat(input, model, map, min_saturation);

    return map;
}

void histProject::get_projection_map_hue_sat(cv::Mat const &input, cv::Mat const &model, cv::Mat &output, int min_saturation)
{
    convert_to_hsv(input, input_hsv_);
    convert_to_hsv(model, model_hsv_);
    if(min_saturation > 0){
        model_saturation_mask_.create(model_hsv_.size(), model_hsv_.depth());
        mix_channels(model_hsv_, model_saturation_mask_, {1, 0});
        cv::threshold(model_saturation_mask_, model_saturation_mask_, min_saturation, 255, cv::THRESH_BINARY);
        calc_histogram<2>({model_hsv_}, model_hist_, {0, 1}, {180, 256}, {{ {0, 180}, {0, 256} }}, model_saturation_mask_);

        map_saturation_mask_.create(input_hsv_.size(), input_hsv_.depth());
        mix_channels(input_hsv_, map_saturation_mask_, {1, 0});
        OCV::calc_back_project<2>({input_hsv_}, {0, 1}, model_hist_, output, {{ {0, 180}, {0, 256} }});
        output &= map_saturation_mask_;
    }else{
        calc_histogram<2>({model_hsv_}, model_hist_, {0, 1}, {180, 256}, {{ {0, 180}, {0, 256} }});
        OCV::calc_back_project<2>({input_hsv_}, {0, 1}, model_hist_, output, {{ {0, 180}, {0, 256} }});
    }
}

void histProject::convert_to_hsv(cv::Mat const &input, cv::Mat &output)
{
    input.copyTo(output);
    if(output.channels() == 4){
        cv::cvtColor(output, output, CV_BGRA2BGR);
    }

    cv::cvtColor(output, output, CV_BGR2HSV);
}

}
