#include "car_benchmark.hpp"
#include "utility.hpp"

#include <ocv_libs/file/utility.hpp>
#include <ocv_libs/ml/utility/split_train_test.hpp>

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
    nn << convolutional_layer<activate>(32, 32, 5, 1, 6, padding::same)
       << max_pooling_layer<activate>(32, 32, 6, 2)
       << convolutional_layer<activate>(16, 16, 5, 6, 16, padding::same)
       << max_pooling_layer<activate>(16, 16, 16, 2)
       << convolutional_layer<activate>(8, 8, 5, 16, 8)
       << fully_connected_layer<activate>(128, 10);

    std::cout << "start learning" << std::endl;

    boost::progress_display disp(static_cast<int>(train_images_.size()));
    boost::timer t;
    int minibatch_size = 10;
    int num_epochs = 30;

    nn.optimizer().alpha *= std::sqrt(minibatch_size);

    // create callback
    auto on_enumerate_epoch = [&](){
        std::cout << t.elapsed() << "s elapsed." << std::endl;
        tiny_cnn::result res = nn.test(test_images_, test_labels_);
        std::cout << res.num_success << "/" << res.num_total << std::endl;

        disp.restart(static_cast<int>(train_images_.size()));
        t.restart();
    };

    auto on_enumerate_minibatch = [&](){
        disp += minibatch_size;
    };

    // training
    nn.train(train_images_, train_labels_, minibatch_size, num_epochs,
             on_enumerate_minibatch, on_enumerate_epoch);

    std::cout << "end training." << std::endl;

    // test and show results
    nn.test(test_images_, test_labels_).print_detail(std::cout);

    // save networks
    std::ofstream ofs("car_weights");
    ofs << nn;
}
