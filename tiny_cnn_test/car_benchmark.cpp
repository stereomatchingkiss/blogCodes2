#include "car_benchmark.hpp"
#include "utility.hpp"

#include <ocv_libs/file/utility.hpp>
#include <ocv_libs/ml/utility/split_train_test.hpp>
#include <ocv_libs/tiny_cnn/trainer.hpp>

#include <boost/progress.hpp>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <fstream>
#include <iostream>

using namespace tiny_cnn;
using namespace tiny_cnn::activation;

namespace{

template<typename Net>
void create_lenet(Net &nn)
{
    using activate = relu;
    nn << convolutional_layer<activate>(32, 32, 5, 1, 6) // C1, 1@32x32-in, 6@28x28-out
       << average_pooling_layer<activate>(28, 28, 6, 2) // S2, 6@28x28-in, 6@14x14-out
       << convolutional_layer<activate>(14, 14, 5, 6, 16) // C3, 6@14x14-in, 16@10x10-in
       << average_pooling_layer<activate>(10, 10, 16, 2) // S4, 16@10x10-in, 16@5x5-out
       << convolutional_layer<activate>(5, 5, 5, 16, 120) // C5, 16@5x5-in, 120@1x1-out
       << fully_connected_layer<activate>(120, 2); // F6, 120-in, 10-out
}

template<typename Net>
void create_minivgg(Net &nn)
{
    using activate = relu;
    dropout_layer drop_00(16*16*6, 0.25);
    dropout_layer drop_01(8*8*16, 0.25);
    dropout_layer drop_02(4*4*24, 0.5);
    nn << convolutional_layer<activate>(32, 32, 3, 1, 6, padding::same)
       << convolutional_layer<activate>(32, 32, 3, 6, 6, padding::same)
       << max_pooling_layer<activate>(32, 32, 6, 2)
          //<< drop_00
       << convolutional_layer<activate>(16, 16, 3, 6, 16, padding::same)
       << convolutional_layer<activate>(16, 16, 3, 16, 16, padding::same)
       << max_pooling_layer<activate>(16, 16, 16, 2)
          //<< drop_01
       << convolutional_layer<activate>(8, 8, 3, 16, 24, padding::same)
       << convolutional_layer<activate>(8, 8, 3, 24, 24, padding::same)
       << max_pooling_layer<activate>(8, 8, 24, 2)
       << drop_02
       << fully_connected_layer<softmax>(384, 2);
}


}

car_benchmark::car_benchmark()
{    
    using namespace ocv::ml;

    std::vector<vec_t> in_img;
    std::vector<label_t> in_label;
    load_data("cars_train_small", 0,
              in_img, in_label);
    load_data("non_cars", 1,
              in_img, in_label);
    std::tie(train_images_, train_labels_,
             test_images_, test_labels_) =
            split_train_test_inplace(in_img, in_label,
                                     0.1, true);
    try{
        train_test();
    }catch(std::exception const &ex){
        std::cout<<ex.what()<<std::endl;
    }
}

void car_benchmark::load_data(std::string const &folder,
                              label_t label,
                              Imgs &imgs, Labels &labels)
{
    auto const names = ocv::file::get_directory_files(folder);
    cv::Mat aug_img;
    for(auto const &name : names){
        try{
            cv::Mat img = cv::imread(folder + "/" + name);
            cv::resize(img, img, {32,32});
            cv::cvtColor(img, img, CV_BGR2GRAY);
            if(!img.empty()){
                imgs.emplace_back(cvmat_to_img<vec_t>(img, 0, 1));
                labels.emplace_back(label);

                cv::flip(img, aug_img, 0); //flip horizontal
                imgs.emplace_back(cvmat_to_img<vec_t>(aug_img, 0, 1));
                labels.emplace_back(label);
            }
        }catch(std::exception const &ex){
            std::cout<<(folder + "/" + name)<<" load fail"<<std::endl;
            std::cout<<"load fail reason : "<<ex.what()<<std::endl;
        }
    }
    std::cout<<"imgs size after loading : "<<imgs.size()<<std::endl;
}

void car_benchmark::train_test()
{
    network<mse, RMSprop> nn;

    create_lenet(nn);

    std::cout << "start learning" << std::endl;

    int const minibatch_size = 10;
    int const num_epochs = 10;

    nn.optimizer().alpha *= std::sqrt(minibatch_size);
    //int const minibatch_size = 5;
    //int const num_epochs = 10;

    //nn.optimizer().alpha *= std::sqrt(train_images_.size());

    ocv::tiny_cnn::trainer tt("car_weights", minibatch_size, num_epochs);
    tt.train_and_test(nn, train_images_, train_labels_,
                      test_images_, test_labels_);
}
