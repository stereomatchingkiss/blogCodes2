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

car_benchmark::car_benchmark()
{
   //parse_cifar10();
   load_data("cars_train_small", 0,
             train_images_, train_labels_);
   ocv::ml::split_train_test_inplace(train_images_, train_labels_,
                                     train_images_, train_labels_,
                                     test_images_, test_labels_,
                                     0.1, false);
   train_test();
}

void car_benchmark::load_data(std::string const &folder,
                              label_t label,
                              Imgs &imgs, Labels &labels)
{
    auto const names = ocv::file::get_directory_files(folder);
    for(auto const &name : names){
        cv::Mat img = cv::imread(folder + "/" + name);
        cv::resize(img, img, {32,32});
        if(!img.empty()){
          imgs.emplace_back(cvmat_to_img<vec_t>(img));
          labels.emplace_back(label);
        }
    }
}

void car_benchmark::train_test()
{
    network<mse, RMSprop> nn; // specify loss-function and learning strategy

    using activate = relu;
    nn << convolutional_layer<activate>(32, 32, 5, 3, 6, padding::same)
       << max_pooling_layer<activate>(32, 32, 6, 2)
       << convolutional_layer<activate>(16, 16, 5, 6, 16, padding::same)
       << max_pooling_layer<activate>(16, 16, 16, 2)
       << convolutional_layer<activate>(8, 8, 5, 16, 8)
       << fully_connected_layer<activate>(128, 10);

    std::cout << "start learning" << std::endl;

    int const minibatch_size = 10;
    int const num_epochs = 20;

    nn.optimizer().alpha *= std::sqrt(minibatch_size);

    ocv::tiny_cnn::trainer tt("car_weights", minibatch_size, num_epochs);
    tt.train_and_test(nn, train_images_, train_labels_,
                      test_images_, test_labels_);
}
