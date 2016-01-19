#ifndef TRAIN_CHARS_HPP
#define TRAIN_CHARS_HPP

#include "utility.hpp"

#include <ocv_libs/core/block_binary_pixel_sum.hpp>

#include <opencv2/core.hpp>
#include <opencv2/ml.hpp>

#include <boost/bimap.hpp>

#include <map>
#include <random>
#include <string>
#include <vector>

enum class map_type;

class train_chars
{
public:
    train_chars(std::string chars_folder,
                std::string result_file,
                map_type mtype = map_type::alpahbet);

    void test_train_result();
    cv::Ptr<cv::ml::StatModel> train();

private:
    using bm_type = boost::bimap<std::string, int>;
    using features_type = std::vector<std::vector<float>>;
    using label_type = std::vector<int>;

    void add_noise(cv::Mat &inout, int label);
    void add_rotation(cv::Mat &inout, int label, double rotation);
    void describe_features();
    void extract_features();
    void extract_feature(cv::Mat const &img, int label);
    void generate_train_number();
    void show_training_results(features_type const &features,
                               label_type const &labels);
    void split_train_test();
    void train_classifier();


    ocv::block_binary_pixel_sum<> bbps_;
    boost::bimap<std::string, int> bm_;
    std::string chars_folder_;
    std::mt19937 generator_; //generator
    std::string result_file_;

    features_type features_;
    cv::Mat features_train_;
    label_type labels_;
    label_type labels_train_;    

    cv::Ptr<cv::ml::StatModel> ml_;
    map_type mtype_;

    size_t augment_ratio_ = 1;
    size_t min_symbol_size_ = 0;
    size_t train_size_ = 0;    
};

#endif
