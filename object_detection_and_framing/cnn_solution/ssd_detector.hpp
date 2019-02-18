#ifndef SSD_DETECTOR_HPP
#define SSD_DETECTOR_HPP

#include "voc_item_type.hpp"

#include <opencv2/dnn.hpp>

#include <map>
#include <vector>

class ssd_detector
{
public:
    struct predict_resutls
    {
        std::vector<double> confidences_;
        std::vector<cv::Rect> boxes_;
    };

    using result_type = std::map<voc_item_type, predict_resutls>;

    /**
     * This constructor will create default class_names same as this
     * example(https://github.com/opencv/opencv/blob/master/samples/dnn/ssd_object_detection.cpp)
     */
    explicit ssd_detector(std::string const &model_bin,
                          std::string const &proto_txt,
                          float confidence = 0.2f,
                          bool swap_rb = true);

    ~ssd_detector();

    result_type detect(cv::Mat const &input);

    void set_class_names(std::vector<std::string> class_names);

private:
    cv::Rect center_crop_rect(cv::Size const in_size) const;

    cv::Mat buffer_;    
    std::vector<std::string> class_names_;
    float confidence_;
    cv::dnn::Net net_;
    bool swap_rb_;
};

#endif // SSD_DETECTOR_HPP
