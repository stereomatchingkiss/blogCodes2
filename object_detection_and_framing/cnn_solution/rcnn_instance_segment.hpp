#ifndef RCNN_INSTANCE_SEGMENT_HPP
#define RCNN_INSTANCE_SEGMENT_HPP

#include "coco_item_type.hpp"

#include <opencv2/dnn.hpp>

#include <map>
#include <vector>

class rcnn_instance_segment
{
public:
    rcnn_instance_segment(std::string const &model_bin,
                          std::string const &proto_txt,
                          float confidence = 0.5f,
                          float mask_threahold = 0.5f);

    void detect(cv::Mat &inout);

    std::string const& get_name(coco_item_type input) const noexcept;

private:
    // Draw the predicted bounding box, colorize and show the mask on the image
    void draw_box(int class_id, float conf, cv::Rect &box, cv::Mat& object_mask, cv::Mat& inout);
    void post_process(std::vector<cv::Mat> const &predict_results, cv::Mat& inout);

    cv::Mat buffer_;
    std::vector<std::string> class_names_;
    std::vector<cv::Scalar> colors_;
    float confidence_;
    float mask_threshold_;
    cv::dnn::Net net_;
    std::vector<std::string> output_names_;
};

#endif // RCNN_INSTANCE_SEGMENT_HPP
