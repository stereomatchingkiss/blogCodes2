#ifndef READ_DATASET_HPP
#define READ_DATASET_HPP

#include <opencv2/core.hpp>

#include <tiny_cnn/tiny_cnn.h>

#include <vector>

class read_dataset
{
public:
    using TinyImg = tiny_cnn::vec_t;

    read_dataset();

    void read_data(std::vector<TinyImg> &train_data,
                   std::vector<tiny_cnn::label_t> &train_labels,
                   std::vector<TinyImg> &test_data,
                   std::vector<tiny_cnn::label_t> &test_labels,
                   bool mean_image_normalization = true);

private:
    std::tuple<std::vector<cv::Mat>, std::vector<size_t>,
    std::vector<cv::Mat>, std::vector<size_t>>
    load_data();

    std::tuple<std::vector<cv::Mat>, std::vector<size_t>,
    std::vector<cv::Mat>, std::vector<size_t>>
    load_data(std::vector<size_t> const &labels);

    void augment_data(std::vector<cv::Mat> &data,
                      std::vector<size_t> &labels) const;
    std::vector<cv::Mat> change_contrast(std::vector<cv::Mat> const &input,
                                         double alpha = 1.5,
                                         double beta = 20) const;
    std::vector<cv::Mat> flip_horizontal(std::vector<cv::Mat> const &input) const;
    std::vector<cv::Mat> flip_vertical(std::vector<cv::Mat> const &input) const;

    size_t read_negative_data();
    size_t read_postive_data();

    void replicate_data(std::vector<size_t> &inout,
                        size_t replicate_num) const;

    std::vector<cv::Mat> loaded_data_; //data loaded from dataset
};

#endif READ_DATASET_HPP
