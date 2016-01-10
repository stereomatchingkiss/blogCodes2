#include "utility.hpp"

#include <opencv2/imgproc.hpp>

#include <vector>

void binarize_image(cv::Mat const &input, cv::Mat &output)
{
    cv::Mat hsv;
    cv::cvtColor(input, hsv, CV_BGR2HSV);
    std::vector<cv::Mat> hsv_split;
    cv::split(hsv, hsv_split);
    cv::Mat intensity;
    cv::GaussianBlur(hsv_split[2], intensity, {7,7}, 0);

    constexpr int blockSize = 11;
    constexpr double offset = 9;
    cv::adaptiveThreshold(intensity, output, 255,
                          cv::ADAPTIVE_THRESH_MEAN_C,
                          cv::THRESH_BINARY_INV,
                          blockSize, offset);
}

cv::Mat binarize_image(const cv::Mat &input)
{
    cv::Mat output;
    binarize_image(input, output);

    return output;
}
