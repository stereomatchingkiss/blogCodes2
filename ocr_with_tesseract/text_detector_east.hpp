#ifndef TEXT_DETECTOR_EAST_HPP
#define TEXT_DETECTOR_EAST_HPP

#include <opencv2/core.hpp>
#include <opencv2/dnn.hpp>

#include <string>
#include <vector>

class text_detector_east
{
public:
    struct result_type
    {
        std::vector<cv::RotatedRect> boxes_;
        std::vector<float> confidences_;
        std::vector<int> indices_;
    };

    explicit text_detector_east(std::string const &model,
                                float confident_threshold = 0.5f,
                                float nms_threshold = 0.4f,
                                cv::Size const &process_size = cv::Size(320, 320));

    result_type detect(cv::Mat const &input);

private:
    cv::Mat blob_;
    float confident_threshold_;
    cv::dnn::Net net_;
    float nms_threshold_;
    std::vector<std::string> out_names_;
    std::vector<cv::Mat> outputs_;
    cv::Size process_size_;
};

#endif // TEXT_DETECTOR_EAST_HPP
