#ifndef YOLOV3_DETECTOR_HPP
#define YOLOV3_DETECTOR_HPP

#include "coco_item_type.hpp"

#include <opencv2/dnn.hpp>

#include <map>
#include <vector>

class yolov3_detector
{
public:
    struct predict_resutls
    {
        std::vector<float> confidences_;
        std::vector<cv::Rect> boxes_;
    };

    using result_type = std::map<coco_item_type, predict_resutls>;

    yolov3_detector(std::string const &model_bin,
                    std::string const &proto_txt,
                    float confidence = 0.2f,
                    cv::Size const &process_size = cv::Size(32*7, 32*7),
                    float nms_threshold = 0.4f);

    result_type detect(cv::Mat const &input);

    std::string const& get_name(coco_item_type input) const noexcept;

private:
    result_type extract_forward_results(cv::Size const &img_size, std::vector<cv::Mat> const &outs) const;

    void init_output_names();

    yolov3_detector::result_type nonmaxima_suppress(yolov3_detector::predict_resutls const &input,
                                                    std::vector<int> const &class_ids) const;

    cv::Mat buffer_;
    std::vector<std::string> class_names_;    
    float confidence_;
    cv::dnn::Net net_;
    float nms_threshold_;
    std::vector<std::string> output_names_;
    cv::Size process_size_;
};

#endif // YOLOV3_DETECTOR_HPP
