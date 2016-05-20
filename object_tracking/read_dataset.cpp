#include "read_dataset.hpp"
#include "get_samples.hpp"
#include "read_stanford40_pose.hpp"
#include "read_usc_pedestrian.hpp"

#include <ocv_libs/tiny_cnn/image_converter.hpp>

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

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

/*template<typename T, typename U>
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
}//*/

}

read_dataset::read_dataset()
{    
}

/*void read_dataset::
read_data(arma::Cube<double> &train_data,
          arma::mat &train_labels,
          arma::Cube<double> &test_data,
          arma::mat &test_labels)
{
    auto data = load_data();
    std::cout<<"cvmat to arma"<<std::endl;
    cvmat_to_arma_cpy<uchar>(std::get<0>(data), train_data);
    std::get<0>(data).clear();
    std::cout<<"cvmat to arma"<<std::endl;
    cvmat_to_arma_cpy<uchar>(std::get<2>(data), test_data);
    std::get<2>(data).clear();

    std::cout<<"set label"<<std::endl;
    test_labels.set_size(2, std::get<3>(data).size());
    for(size_t i = 0; i != std::get<3>(data).size(); ++i){
        test_labels.col(i)(std::get<3>(data)[i]) = 1;
    }

    train_labels.set_size(2, std::get<1>(data).size());
    for(size_t i = 0; i != std::get<1>(data).size(); ++i){
        train_labels.col(i)(std::get<1>(data)[i]) = 1;
    }
}//*/

void read_dataset::
read_data(std::vector<TinyImg> &train_data,
          std::vector<tiny_cnn::label_t> &train_labels,
          std::vector<TinyImg> &test_data,
          std::vector<tiny_cnn::label_t> &test_labels)
{
    using namespace ocv::tiny_cnn;

    auto data = load_data();
    train_data.clear();
    test_data.clear();
    std::cout<<"cvmat to arma"<<std::endl;
    cvmat_to_img(std::get<0>(data), train_data);
    std::get<0>(data).clear();

    std::cout<<"cvmat to arma"<<std::endl;
    cvmat_to_img(std::get<2>(data), test_data);
    std::get<2>(data).clear();

    std::cout<<"set label"<<std::endl;
    test_labels.resize(std::get<3>(data).size());
    for(size_t i = 0; i != std::get<3>(data).size(); ++i){
        test_labels[i] = std::get<3>(data)[i];
    }

    train_labels.resize(std::get<1>(data).size());
    for(size_t i = 0; i != std::get<1>(data).size(); ++i){
        train_labels[i] = std::get<1>(data)[i];
    }
}

std::tuple<std::vector<cv::Mat>, std::vector<size_t>,
std::vector<cv::Mat>, std::vector<size_t>>
read_dataset::load_data()
{
    loaded_data_.clear();
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

    return std::make_tuple(std::move(train_data),
                           std::move(train_labels),
                           std::move(test_data),
                           std::move(test_labels));
}

void read_dataset::augment_data(std::vector<cv::Mat> &data,
                                std::vector<size_t> &labels) const
{
    std::cout<<"aug horizontal"<<std::endl;
    auto aug_horizontal = flip_horizontal(data);
    auto aug_contrast = change_contrast(data, 1.5, 20);
    boost::copy(aug_horizontal, std::back_inserter(data));
    std::cout<<"aug contrast"<<std::endl;
    boost::copy(aug_contrast, std::back_inserter(data));

    replicate_data(labels, 2);
}

std::vector<cv::Mat>
read_dataset::change_contrast(std::vector<cv::Mat> const &input,
                              double alpha,
                              double beta) const
{
    std::vector<cv::Mat> aug_contrast(input.size());
    for(size_t i = 0; i != input.size(); ++i){
        aug_contrast[i] = input[i] * alpha + beta;
    }
    return aug_contrast;
}

std::vector<cv::Mat> read_dataset::
flip_horizontal(std::vector<cv::Mat> const &input) const
{
    std::vector<cv::Mat> aug_horizon(input.size());
    for(size_t i = 0; i != input.size(); ++i){
        cv::flip(input[i], aug_horizon[i], 0);
    }
    return aug_horizon;
}

size_t read_dataset::read_negative_data()
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

size_t read_dataset::read_postive_data()
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

void read_dataset::replicate_data(std::vector<size_t> &inout,
                                  size_t replicate_num) const
{
    auto const ori_input = inout;
    for(size_t i = 0; i != replicate_num; ++i){
        boost::copy(ori_input, std::back_inserter(inout));
    }
}
