#ifndef TRAIN_CHARS_HPP
#define TRAIN_CHARS_HPP

#include <ocv_libs/core/block_binary_pixel_sum.hpp>

#include <opencv2/core.hpp>

#include <map>
#include <string>
#include <vector>

namespace cv{

namespace ml{

class StatModel;

}

}

class train_chars
{
public:
    train_chars(std::string chars_folder,
                std::string result_folder);

    cv::Ptr<cv::ml::StatModel> train();

private:
    void extract_features();

    cv::Ptr<cv::ml::StatModel> ml_;

    ocv::block_binary_pixel_sum<> bbps_;
    std::string chars_folder_;
    std::string result_folder_;

    cv::Mat features_;
    std::vector<int> labels_;
    std::map<std::string, int> labels_to_int_;
    void train_svm();
};

#endif
