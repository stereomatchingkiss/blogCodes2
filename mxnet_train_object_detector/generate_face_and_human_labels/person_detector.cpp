#include "person_detector.hpp"

#include "general_settings_param_parser.hpp"

#include <mxnet_cpp_object_detection/object_detector.hpp>

person_detector::person_detector(const QString &setting_file_location)
{
    general_settings_param_parser const parser(setting_file_location);

    cv::Size const process_size;
    object_detector_.reset(new object_detector(parser.get_model_params().toStdString(),
                                               parser.get_model_symbols().toStdString(),
                                               mxnet::cpp::Context::gpu(0),
                                               parser.get_detector_process_size()));

    std::vector<object_detector_filter::item_type> types_to_detect;
    types_to_detect.emplace_back(object_detector_filter::item_type::person);
    filter_.reset(new object_detector_filter(types_to_detect,
                                             parser.get_detector_process_size(),                                             
                                             static_cast<float>(parser.get_detect_confidence())));
}

person_detector::~person_detector()
{

}

std::vector<object_detector_filter::result_type> person_detector::detect(const cv::Mat &img)
{
    object_detector_->forward(img);    
    return filter_->filter(object_detector_->get_outputs(), cv::Size(img.cols, img.rows));
}
