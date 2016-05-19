#include "cnn_train_test.hpp"
#include "get_samples.hpp"
#include "read_stanford40_pose.hpp"
#include "read_usc_pedestrian.hpp"

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <mlpack/core/util/split_data.hpp>

#include <boost/range/algorithm.hpp>

#include <algorithm>
#include <iostream>
#include <numeric>
#include <random>

namespace{

void train_test_split(std::vector<cv::Mat> &data,
                      std::vector<size_t> &data_labels,
                      std::vector<cv::Mat> &train,
                      std::vector<size_t> &train_labels,
                      std::vector<cv::Mat> &test,
                      std::vector<size_t> &test_labels,
                      double ratio = 0.2,
                      unsigned int seed = 0)
{
    std::vector<size_t> orders(data.size());
    std::iota(std::begin(orders), std::end(orders), 0);
    std::shuffle(std::begin(orders), std::end(orders),
                 std::mt19937(seed));

    size_t const test_size = static_cast<size_t>(data.size() * ratio);
    test.resize(test_size);
    test_labels.resize(test_size);
    for(size_t i  = 0; i != test_size; ++i){
        test[i] = data[i];
        test_labels[i] = data_labels[i];
    }

    size_t const train_size = data.size() - test_size;
    train.resize(train_size);
    train_labels.resize(train_size);
    for(size_t i  = test_size; i != train_size + test_size; ++i){
        train[i] = data[i];
        train_labels[i] = data_labels[i];
    }
}

std::string const dataset_prefix("C:/Users/yyyy/Qt/computer_vision_dataset");

auto resize_to_gray = [](cv::Mat &input)
{
    cv::resize(input, input, {32,32});
    if(input.channels() == 3){
        cv::cvtColor(input, input, CV_BGR2GRAY);
    }
};

void display_img(std::vector<cv::Mat> const &imgs)
{
    for(auto const &img : imgs){
        cv::imshow("ooo", img);
        int const key = cv::waitKey();
        if(key == 'q'){
            break;
        }
    }
}

template<typename T, typename U>
void cvmat_to_arma_cpy(std::vector<cv::Mat> const &input,
                       std::vector<arma::Mat<U>> &output)
{
    output.clear();
    output.reserve(input.size());
    for(size_t i = 0; i != input.size(); ++i){
        output[i].set_size(input[i].cols, input[i].rows);
        cv::Mat temp;
        if(input[i].isContinuous()){
            temp = input[i];
        }else{
            temp = input[i].clone();
        }
        std::copy(temp.ptr<T>(0), temp.ptr<T>(0) + temp.total(),
                  output[i].memptr());
    }
}

}

cnn_train_test::cnn_train_test()
{
    auto const positive_size = read_postive_data();
    read_negative_data();
    std::vector<size_t> labels(loaded_data_.size());
    std::fill(std::begin(labels), std::begin(labels) + positive_size, 1);
    std::fill(std::begin(labels) + positive_size, std::end(labels), 0);

    std::vector<cv::Mat> train_data;
    std::vector<cv::Mat> test_data;
    std::vector<size_t> train_labels;
    std::vector<size_t> test_labels;
    train_test_split(loaded_data_, labels, train_data,
                     train_labels, test_data, test_labels);
    augment_data(train_data, train_labels);

    loaded_data_.clear();
    cvmat_to_arma_cpy<uchar>(train_data, train_data_);
    train_data.clear();
    cvmat_to_arma_cpy<uchar>(test_data, test_data_);
    test_data.clear();

    test_labels_.set_size(test_labels.size());
    for(size_t i = 0; i != test_labels.size(); ++i){
        test_labels_[i] = test_labels[i];
    }

    train_labels_.set_size(train_labels.size());
    for(size_t i = 0; i != train_labels.size(); ++i){
        train_labels_[i] = train_labels[i];
    }
}

void cnn_train_test::augment_data(std::vector<cv::Mat> &data,
                                  std::vector<size_t> &labels) const
{
    boost::copy(flip_horizontal(data), std::back_inserter(data));
    boost::copy(change_contrast(data, 1.5, 20), std::back_inserter(data));
    //boost::copy(change_contrast(data, 1.5, 50), std::back_inserter(data));

    replicate_data(labels, 2);
}

std::vector<cv::Mat>
cnn_train_test::change_contrast(std::vector<cv::Mat> const &input,
                                double alpha,
                                double beta) const
{
    std::vector<cv::Mat> aug_contrast(input.size());
    for(size_t i = 0; i != input.size(); ++i){
        aug_contrast[i] = input[i] * alpha + beta;
    }
    return aug_contrast;
}

std::vector<cv::Mat> cnn_train_test::
flip_horizontal(std::vector<cv::Mat> const &input) const
{
    std::vector<cv::Mat> aug_horizon(input.size());
    for(size_t i = 0; i != input.size(); ++i){
        cv::flip(input[i], aug_horizon[i], 0);
    }
    return aug_horizon;
}

size_t cnn_train_test::read_negative_data()
{
    auto const origin_size = loaded_data_.size();

    get_caltech_bg(loaded_data_,
                   dataset_prefix + "/background/caltech_bg",
                   859, resize_to_gray);//*/
    get_indoor_scene_cvpr2009(loaded_data_,
                              dataset_prefix + "/background/indoorCVPR_09/no_human",
                              1141, resize_to_gray);//*/
    std::cout<<"negative imgs size : "<<loaded_data_.size()<<std::endl;
    //display_img(data_);

    return loaded_data_.size() - origin_size;
}

size_t cnn_train_test::read_postive_data()
{    
    auto const origin_size = loaded_data_.size();

    get_stanford40_pose(loaded_data_,
                        dataset_prefix + "/human_pose/Stanford40",
                        1184, resize_to_gray);//*/
    //313
    get_usc_pedestrian(loaded_data_,
                       dataset_prefix + "/pedestrian/USCPedestrianSetA",
                       205, resize_to_gray);//*/
    //271, 271 + 313 = 584
    get_usc_pedestrian(loaded_data_,
                       dataset_prefix + "/pedestrian/USCPedestrianSetB",
                       54, resize_to_gray);//*/
    //232, 584 + 232 = 816
    get_usc_pedestrian(loaded_data_,
                       dataset_prefix + "/pedestrian/USCPedestrianSetC",
                       100, resize_to_gray);//*/
    std::cout<<"positive imgs size : "<<loaded_data_.size()<<std::endl;
    //display_img(data_);

    return loaded_data_.size() - origin_size;
}

void cnn_train_test::replicate_data(std::vector<size_t> &inout,
                                    size_t replicate_num) const
{
    auto const ori_input = inout;
    for(size_t i = 0; i != replicate_num; ++i){
        boost::copy(ori_input, std::back_inserter(inout));
    }
}
