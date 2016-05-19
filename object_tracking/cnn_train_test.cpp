#include "cnn_train_test.hpp"
#include "get_samples.hpp"
#include "read_stanford40_pose.hpp"
#include "read_usc_pedestrian.hpp"

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <mlpack/core/util/split_data.hpp>
#include <mlpack/methods/ann/activation_functions/logistic_function.hpp>

#include <mlpack/methods/ann/layer/one_hot_layer.hpp>
#include <mlpack/methods/ann/layer/conv_layer.hpp>
#include <mlpack/methods/ann/layer/pooling_layer.hpp>
#include <mlpack/methods/ann/pooling_rules/max_pooling.hpp>
#include <mlpack/methods/ann/layer/softmax_layer.hpp>
#include <mlpack/methods/ann/layer/bias_layer.hpp>
#include <mlpack/methods/ann/layer/linear_layer.hpp>
#include <mlpack/methods/ann/layer/base_layer.hpp>

#include <mlpack/methods/ann/performance_functions/mse_function.hpp>
#include <mlpack/core/optimizers/rmsprop/rmsprop.hpp>

#include <mlpack/methods/ann/init_rules/random_init.hpp>
#include <mlpack/methods/ann/cnn.hpp>

#include <boost/range/algorithm.hpp>

#include <algorithm>
#include <iostream>
#include <numeric>
#include <random>

namespace{

void train_test_split(std::vector<cv::Mat> const &data,
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

    std::cout<<"split test data"<<std::endl;
    size_t const test_size = static_cast<size_t>(data.size() * ratio);
    std::cout<<"test size : "<<test_size<<std::endl;
    test.resize(test_size);
    test_labels.resize(test_size);
    for(size_t i  = 0; i != test_size; ++i){
        test[i] = data[i];
        test_labels[i] = data_labels[i];
    }

    std::cout<<"split train data"<<std::endl;
    size_t const train_size = data.size() - test_size;
    std::cout<<"train size : "<<train_size<<std::endl;
    train.resize(train_size);
    train_labels.resize(train_size);
    for(size_t i  = test_size, j = 0; i != data.size(); ++i, ++j){
        train[j] = data[i];
        train_labels[j] = data_labels[i];
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
                       arma::Cube<U> &output)
{
    if(!input.empty()){
        output = arma::Cube<U>(input[0].rows, input[0].cols, input.size());
        for(size_t i = 0; i != input.size(); ++i){
            cv::Mat temp;
            if(input[i].isContinuous()){
                temp = input[i];
            }else{
                temp = input[i].clone();
            }
            std::copy(temp.ptr<T>(0), temp.ptr<T>(0) + temp.total(),
                      output.slice(i).memptr());
        }
    }
}

}

cnn_train_test::cnn_train_test()
{
    auto const positive_size = read_postive_data();
    read_negative_data();
    std::vector<size_t> labels(loaded_data_.size());
    //std::cout<<"labels size : "<<labels.size()<<std::endl;
    std::fill(std::begin(labels), std::begin(labels) + positive_size, 1);
    std::fill(std::begin(labels) + positive_size, std::end(labels), 0);

    std::vector<cv::Mat> train_data;
    std::vector<cv::Mat> test_data;
    std::vector<size_t> train_labels;
    std::vector<size_t> test_labels;
    std::cout<<"train test split"<<std::endl;
    train_test_split(loaded_data_, labels, train_data,
                     train_labels, test_data, test_labels);
    std::cout<<"augment data"<<std::endl;
    augment_data(train_data, train_labels);

    loaded_data_.clear();
    std::cout<<"cvmat to arma"<<std::endl;
    cvmat_to_arma_cpy<uchar>(train_data, train_data_);
    train_data.clear();
    std::cout<<"cvmat to arma"<<std::endl;
    cvmat_to_arma_cpy<uchar>(test_data, test_data_);
    test_data.clear();

    std::cout<<"set label"<<std::endl;
    test_labels_.set_size(test_labels.size());
    for(size_t i = 0; i != test_labels.size(); ++i){
        test_labels_[i] = test_labels[i];
    }

    train_labels_.set_size(train_labels.size());
    for(size_t i = 0; i != train_labels.size(); ++i){
        train_labels_[i] = train_labels[i];
    }

    train_test_00();
}

void cnn_train_test::train_test_00()
{
    using namespace mlpack::ann;
    using namespace mlpack::optimization;

    using FullConvLayer =
    ConvLayer<NaiveConvolution<FullConvolution>,
    NaiveConvolution<FullConvolution>,
    NaiveConvolution<FullConvolution>>;

    ConvLayer<> conv_layer_0(1, 8, 1, 1);
    ConvLayer<> conv_layer_1(8, 8, 1, 3);
    ConvLayer<> conv_layer_2(8, 8, 3, 1);
    BiasLayer2D<> bias_layer_0(8);
    BaseLayer2D<RectifierFunction> base_layer_0;
    PoolingLayer<MaxPooling> pooling_layer_0(2);

    /*ConvLayer<> conv_layer_2(8, 16, 1, 1);
    ConvLayer<> conv_layer_3(16, 16, 3, 3);
    BiasLayer2D<> bias_layer_1(8);
    BaseLayer2D<RectifierFunction> base_layer_1;
    PoolingLayer<MaxPooling> pooling_layer_1(2);*/

    LinearMappingLayer<> linear_layer(15*15*8, 2);
    BiasLayer<> bias_layer_out(2);
    SoftmaxLayer<> softmax_layer;

    OneHotLayer output_layer;
    auto modules = std::tie(conv_layer_0, conv_layer_1, conv_layer_2,
                            bias_layer_0, base_layer_0, pooling_layer_0,
                            linear_layer, bias_layer_out, softmax_layer);
    CNN<decltype(modules), decltype(output_layer),
            RandomInitialization, MeanSquaredErrorFunction>
            net(modules, output_layer);
    bias_layer_0.Weights().zeros();

    std::cout<<"convert labels"<<std::endl;
    arma::mat Y = arma::zeros<arma::mat>(2, train_data_.n_slices);
    std::cout<<"train data size, train label size "<<
               train_data_.n_slices<<","<<
               train_labels_.n_elem<<std::endl;
    for(size_t i = 0; i != train_data_.n_slices; ++i){
        if(train_labels_(i) == 0){
            Y.col(i)(0) = 1;
        }else{
            Y.col(i)(1) = 1;
        }
    }
    RMSprop<decltype(net)> opt(net, 0.01, 0.88, 1e-8,
                               10 * train_data_.n_slices, 0);
    std::cout<<"train network"<<std::endl;
    net.Train(train_data_, Y, opt);
}

void cnn_train_test::augment_data(std::vector<cv::Mat> &data,
                                  std::vector<size_t> &labels) const
{
    std::cout<<"aug horizontal"<<std::endl;
    auto aug_horizontal = flip_horizontal(data);
    auto aug_contrast = change_contrast(data, 1.5, 20);
    boost::copy(aug_horizontal, std::back_inserter(data));
    std::cout<<"aug contrast"<<std::endl;
    boost::copy(aug_contrast, std::back_inserter(data));
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
    std::cout<<"negative imgs size : "<<
               loaded_data_.size() - origin_size<<std::endl;
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
    std::cout<<"positive imgs size : "<<
               loaded_data_.size() - origin_size<<std::endl;
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
