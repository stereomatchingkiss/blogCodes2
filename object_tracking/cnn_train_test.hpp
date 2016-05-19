#ifndef CNN_TRAIN_TEST_HPP
#define CNN_TRAIN_TEST_HPP

#include <mlpack/core.hpp>

#include <opencv2/core.hpp>

#include <vector>

class cnn_train_test
{
public:
    cnn_train_test();

    void train_test_00();

private:
    void augment_data(std::vector<cv::Mat> &data,
                      std::vector<size_t> &labels) const;
    std::vector<cv::Mat> change_contrast(std::vector<cv::Mat> const &input,
                                         double alpha = 1.5,
                                         double beta = 20) const;
    std::vector<cv::Mat> flip_horizontal(std::vector<cv::Mat> const &input) const;

    size_t read_negative_data();
    size_t read_postive_data();

    void replicate_data(std::vector<size_t> &inout,
                        size_t replicate_num) const;

    std::vector<cv::Mat> loaded_data_; //data loaded from dataset
    arma::Cube<float> train_data_;
    arma::Row<size_t> train_labels_;
    arma::Cube<float> test_data_;
    arma::Row<size_t> test_labels_;
};

#endif CNN_TRAIN_TEST_HPP
