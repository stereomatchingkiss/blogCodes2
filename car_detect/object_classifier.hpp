#ifndef OBJECT_CLASSIFIER_H
#define OBJECT_CLASSIFIER_H

#include <opencv2/dnn.hpp>

#include <string>

class object_classifier
{
public:
    object_classifier(std::string model_txt, std::string model_bin,
                      std::string class_name, cv::Size const &target_size);

    void populate_net(std::string const &model_txt,
                      std::string const &model_bin,
                      std::string const &class_name, cv::Size const &target_size);

    std::tuple<std::vector<std::string> const&,int, double> const
    predict(cv::Mat const &img);

private:
    void get_max_class(cv::dnn::Blob &probBlob,
                       int *classId, double *classProb);
    void populate_net();
    void read_class_names();

    std::string class_name_;
    std::vector<std::string> class_names_;
    std::string model_bin_;
    std::string model_txt_;

    cv::dnn::Net net_;
    cv::Mat resize_img_;
    cv::Size target_size_;
};

#endif // OBJECT_CLASSIFIER_H
