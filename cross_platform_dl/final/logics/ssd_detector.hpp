#ifndef SSD_DETECTOR_HPP
#define SSD_DETECTOR_HPP

#include <opencv2/dnn.hpp>

#include <map>
#include <vector>

class ssd_detector
{
public:
    using result_type = std::pair<std::map<std::string, std::vector<cv::Rect>>, cv::Point>;

    /**
     * @param model_bin binary file of caffe model
     * @param proto_txt cofiguration file of caffe model
     * @param class_names names of the class
     * @param swap_rb flag which indicates that swap first and last channels
     * in 3-channel image is necessary.
     */
    explicit ssd_detector(std::string const &model_bin, std::string const &proto_txt,
                          std::vector<std::string> class_names, bool swap_rb = true);
    /**
     * This constructor will create default class_names same as this
     * example(https://github.com/opencv/opencv/blob/master/samples/dnn/ssd_object_detection.cpp)
     */
    explicit ssd_detector(std::string const &model_bin, std::string const &proto_txt,
                          bool swap_rb = true);

    ~ssd_detector();

    result_type detect(cv::Mat const &input, float min_confident = 0.2);

    void set_class_names(std::vector<std::string> class_names);

private:
    cv::Rect center_crop_rect(cv::Size const in_size) const;

    cv::Mat buffer_;
    std::vector<std::string> class_names_;
    cv::dnn::Net net_;
    bool swap_rb_;
};

#endif // SSD_DETECTOR_HPP
