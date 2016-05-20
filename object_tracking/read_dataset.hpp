#ifndef READ_DATASET_HPP
#define READ_DATASET_HPP

#include <mlpack/core.hpp>

#include <opencv2/core.hpp>

#include <vector>

class read_dataset
{
public:
    read_dataset();

    void read_data(arma::Cube<double> &train_data,
                   arma::mat &train_labels,
                   arma::Cube<double> &test_data,
                   arma::mat &test_labels);
    //void read_data();

private:
    std::tuple<std::vector<cv::Mat>, std::vector<size_t>,
    std::vector<cv::Mat>, std::vector<size_t>>
    load_data();

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
};

#endif READ_DATASET_HPP
