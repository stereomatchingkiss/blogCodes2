#ifndef TRAIN_CHARS_HPP
#define TRAIN_CHARS_HPP

#include <ocv_libs/core/block_binary_pixel_sum.hpp>

#include <opencv2/core.hpp>
#include <opencv2/ml.hpp>

#include <map>
#include <string>
#include <vector>

class train_chars
{
public:
    train_chars(std::string chars_folder,
                std::string result_folder);

    cv::Ptr<cv::ml::StatModel> train();

private:
    void describe_features();
    void train_svm();
    void generate_train_number();

    cv::Ptr<cv::ml::StatModel> ml_;

    ocv::block_binary_pixel_sum<> bbps_;
    std::string chars_folder_;
    std::string result_folder_;

    std::vector<std::vector<float>> features_;
    cv::Mat features_train_;
    cv::Mat features_validate_;
    std::vector<int> labels_;
    std::vector<int> labels_train_;
    std::vector<int> labels_validate_;
    std::map<std::string, int> labels_to_int_;

    size_t train_size_ = 0;
    size_t validate_size_ = 0;
    void extract_features();
};

#endif
