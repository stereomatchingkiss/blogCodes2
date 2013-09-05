#ifndef HISTBACKPROJECTNOUI_HPP
#define HISTBACKPROJECTNOUI_HPP

#include <opencv2/core/core.hpp>

class histBackProjectNoUI
{
public:
    std::pair<cv::Mat, cv::Mat> custom_back_project(int histogram_dimension);
    std::pair<cv::Mat, cv::Mat> openCV_back_project(int histogram_dimension);

    void run();

private:
    cv::Mat convert_to_hsv(cv::Mat const &input) const;

    cv::Mat generate_probability_map(int histogram_dimension);
    void generate_ratio(int histogram_dimension);    

    void remove_noise(cv::Mat &probability_map);
    cv::Mat separate_target_and_background(cv::Mat &probability_map);

private:
    cv::Mat input_;
    cv::Mat input_hsv_;
    cv::Mat mask_;
    cv::Mat model_;
    cv::Mat model_hsv_;
    cv::Mat probability_map_;
    cv::Mat ratio_;    
};


#endif // HISTBACKPROJECTNOUI_HPP
