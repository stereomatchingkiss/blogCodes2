#include "tiny_cnn_trainer.hpp"
#include "read_dataset.hpp"

#include <ocv_libs/ml/utility/shuffle_data.hpp>
#include <ocv_libs/tiny_cnn/image_converter.hpp>
#include <ocv_libs/tiny_cnn/trainer.hpp>

using namespace tiny_cnn;
using namespace tiny_cnn::activation;

using NetType = network<mse, adam>;

namespace{

void create_arch_00(NetType &nn)
{
    using activate = relu;

    //658/800, 10 epoch, 128 batch, 3.5 alpha, relu, network<mse, adagrad>, 9600, augment h, contrast 0
    //626/800, 10 epoch, 64 batch, 4.5 alpha, relu, network<mse, adam>, 9600, augment h, contrast 0
    //683/800, 10 epoch, 64 batch, 4.25 alpha, relu, network<mse, adam>, 9600, augment h, contrast 0
    //676/800, 10 epoch, 64 batch, 4.0 alpha, relu, network<mse, adam>, 9600, augment h, contrast 0
    //645/800, 10 epoch, 64 batch, 3.75 alpha, relu, network<mse, adam>, 9600, augment h, contrast 0
    nn << convolutional_layer<activate>(32, 32, 1, 1, 1, 8, padding::same)
       << convolutional_layer<activate>(32, 32, 3, 1, 8, 8, padding::same)
       << convolutional_layer<activate>(32, 32, 1, 3, 8, 8, padding::same)
       << max_pooling_layer<activate>(32, 32, 8, 2)
       << convolutional_layer<activate>(16, 16, 1, 1, 8, 16, padding::same)
       << convolutional_layer<activate>(16, 16, 3, 1, 16, 16, padding::same)
       << convolutional_layer<activate>(16, 16, 1, 3, 16, 16, padding::same)
       << max_pooling_layer<activate>(16, 16, 16, 2)
       << convolutional_layer<activate>(8, 8, 1, 1, 16, 32, padding::same)
       << convolutional_layer<activate>(8, 8, 3, 1, 32, 32, padding::same)
       << convolutional_layer<activate>(8, 8, 1, 3, 32, 32, padding::same)
       << max_pooling_layer<activate>(8, 8, 32, 2)
       << fully_connected_layer<softmax>(4*4*32, 2);
}

void create_arch_01(NetType &nn)
{
    using activate = relu;

    //675/800, 10 epoch, 64 batch, 4.5 alpha, relu, network<mse, adam>, 3200, augment h, contrast 0
    //646/800, 10 epoch, 64 batch, 4.25 alpha, relu, network<mse, adam>, 3200, augment h, contrast 0
    //703/800, 10 epoch, 64 batch, 4.0 alpha, relu, network<mse, adam>, 3200, augment h, contrast 0
    //700/800, 10 epoch, 64 batch, 3.75 alpha, relu, network<mse, adam>, 3200, augment h, contrast 0

    //673/800, 10 epoch, 64 batch, 4.5 alpha, relu, network<mse, adam>, 3200, augment h/v, contrast 0
    //663/800, 10 epoch, 64 batch, 4.25 alpha, relu, network<mse, adam>, 3200, augment h/v, contrast 0
    //691/800, 10 epoch, 64 batch, 4.0 alpha, relu, network<mse, adam>, 3200, augment h/v, contrast 0
    //694/800, 10 epoch, 64 batch, 3.75 alpha, relu, network<mse, adam>, 3200, augment h/v, contrast 0

    //666/800, 10 epoch, 64 batch, 4.5 alpha, relu, network<mse, adam>, 3200, augment h/v, mean image norm
    //592/800, 10 epoch, 64 batch, 4.25 alpha, relu, network<mse, adam>, 3200, augment h/v, mean image norm
    //629/800, 10 epoch, 64 batch, 4.0 alpha, relu, network<mse, adam>, 3200, augment h/v, mean image norm
    //668/800, 10 epoch, 64 batch, 3.75 alpha, relu, network<mse, adam>, 3200, augment h/v, mean image norm

    //668/800, 10 epoch, 64 batch, 4.5 alpha, relu, network<mse, adam>, 3200, augment h/v
    //674/800, 10 epoch, 64 batch, 4.25 alpha, relu, network<mse, adam>, 3200, augment h/v
    //689/800, 10 epoch, 64 batch, 4.0 alpha, relu, network<mse, adam>, 3200, augment h/v
    //653/800, 10 epoch, 64 batch, 3.75 alpha, relu, network<mse, adam>, 3200, augment h/v

    //679/800, 10 epoch, 64 batch, 4.5 alpha, relu, network<mse, adam>, 3200, augment h/v, contrast 0,1
    //670/800, 10 epoch, 64 batch, 4.25 alpha, relu, network<mse, adam>, 3200, augment h/v, contrast 0,1
    //698/800, 10 epoch, 64 batch, 4.0 alpha, relu, network<mse, adam>, 3200, augment h/v, contrast 0,1
    //709/800, 10 epoch, 64 batch, 3.75 alpha, relu, network<mse, adam>, 3200, augment h/v, contrast 0,1
    nn << convolutional_layer<activate>(32, 32, 3, 1, 12, padding::same)
       << max_pooling_layer<activate>(32, 32, 12, 2)
       << convolutional_layer<activate>(16, 16, 3, 12, 18, padding::same)
       << max_pooling_layer<activate>(16, 16, 18, 2)
       << convolutional_layer<activate>(8, 8, 5, 18, 24)
       << fully_connected_layer<softmax>(4*4*24, 2);//*/
}

void create_arch_02(NetType &nn)
{
    using activate = relu;

    //643/800, 10 epoch, 64 batch, 4.5 alpha, relu, network<mse, adam>, 3200, augment h, contrast 0
    //683/800, 10 epoch, 64 batch, 4.25 alpha, relu, network<mse, adam>, 3200, augment h, contrast 0
    //701/800, 10 epoch, 64 batch, 4.0 alpha, relu, network<mse, adam>, 3200, augment h, contrast 0
    //666/800, 10 epoch, 64 batch, 3.75 alpha, relu, network<mse, adam>, 3200, augment h, contrast 0
    nn << convolutional_layer<activate>(32, 32, 3, 1, 1, 12, padding::same)
       << convolutional_layer<activate>(32, 32, 1, 3, 12, 12, padding::same)
       << max_pooling_layer<activate>(32, 32, 12, 2)
       << convolutional_layer<activate>(16, 16, 3, 1, 12, 18, padding::same)
       << convolutional_layer<activate>(16, 16, 1, 3, 18, 18, padding::same)
       << max_pooling_layer<activate>(16, 16, 18, 2)
       << convolutional_layer<activate>(8, 8, 5, 1, 18, 24)
       << convolutional_layer<activate>(4, 8, 1, 5, 24, 24)
       << fully_connected_layer<softmax>(4*4*24, 2);//*/
}

void create_arch_03(NetType &nn)
{
    using activate = relu;

    //696/800, 10 epoch, 64 batch, 4.5 alpha, relu, network<mse, adam>, 3200, augment h, contrast 0
    //680/800, 10 epoch, 64 batch, 4.25 alpha, relu, network<mse, adam>, 3200, augment h, contrast 0
    //669/800, 10 epoch, 64 batch, 4.0 alpha, relu, network<mse, adam>, 3200, augment h, contrast 0
    //699/800, 10 epoch, 64 batch, 3.75 alpha, relu, network<mse, adam>, 3200, augment h, contrast 0
    nn << convolutional_layer<activate>(32, 32, 3, 1, 1, 12, padding::same)
       << convolutional_layer<activate>(32, 32, 1, 3, 12, 18, padding::same)
       << max_pooling_layer<activate>(32, 32, 18, 2)
       << convolutional_layer<activate>(16, 16, 3, 1, 18, 24, padding::same)
       << convolutional_layer<activate>(16, 16, 1, 3, 24, 30, padding::same)
       << max_pooling_layer<activate>(16, 16, 30, 2)
       << convolutional_layer<activate>(8, 8, 5, 1, 30, 36)
       << convolutional_layer<activate>(4, 8, 1, 5, 36, 42)
       << fully_connected_layer<softmax>(4*4*42, 2);//*/
}

void create_arch_04(NetType &nn)
{
    using activate = relu;

    //401/800, 10 epoch, 64 batch, 4.5 alpha, relu, network<mse, adam>, 3200, augment h, contrast 0
    //400/800, 10 epoch, 64 batch, 4.25 alpha, relu, network<mse, adam>, 3200, augment h, contrast 0
    //401/800, 10 epoch, 64 batch, 4.0 alpha, relu, network<mse, adam>, 3200, augment h, contrast 0
    //401/800, 10 epoch, 64 batch, 3.75 alpha, relu, network<mse, adam>, 3200, augment h, contrast 0
    //637/800, 10 epoch, 64 batch, 0.75 alpha, relu, network<mse, adam>, 3200, augment h, contrast 0
    nn << convolutional_layer<activate>(32, 32, 3, 1, 1, 12, padding::same)
       << convolutional_layer<activate>(32, 32, 1, 3, 12, 18, padding::same)
       << dropout_layer(32*32*18, 0.25)
       << max_pooling_layer<activate>(32, 32, 18, 2)
       << convolutional_layer<activate>(16, 16, 3, 1, 18, 24, padding::same)
       << convolutional_layer<activate>(16, 16, 1, 3, 24, 30, padding::same)
       << dropout_layer(16*16*30, 0.25)
       << max_pooling_layer<activate>(16, 16, 30, 2)
       << convolutional_layer<activate>(8, 8, 5, 1, 30, 36)
       << convolutional_layer<activate>(4, 8, 1, 5, 36, 42)
       << dropout_layer(4*4*42, 0.5)
       << fully_connected_layer<softmax>(4*4*42, 2);//*/
}

void create_arch_05(NetType &nn)
{
    using activate = relu;

    //674/800, 10 epoch, 64 batch, 4.5 alpha, relu, network<mse, adam>, 3200, augment h, contrast 0
    //684/800, 10 epoch, 64 batch, 4.25 alpha, relu, network<mse, adam>, 3200, augment h, contrast 0
    //641/800, 10 epoch, 64 batch, 4.0 alpha, relu, network<mse, adam>, 3200, augment h, contrast 0
    //625/800, 10 epoch, 64 batch, 3.75 alpha, relu, network<mse, adam>, 3200, augment h, contrast 0
    nn << convolutional_layer<activate>(32, 32, 3, 1, 12, padding::same)
       << max_pooling_layer<activate>(32, 32, 12, 2)
       << convolutional_layer<activate>(16, 16, 3, 12, 18, padding::same)
       << max_pooling_layer<activate>(16, 16, 18, 2)
       << convolutional_layer<activate>(8, 8, 3, 18, 24)
       << fully_connected_layer<softmax>(6*6*24, 2);//*/
}

}

void tiny_cnn_tainer::train()
{
    try{
        read_dataset rd;
        rd.read_data(train_images_, train_labels_,
                     test_images_, test_labels_, false);
        std::default_random_engine re(100);
        ocv::ml::shuffles(train_images_, train_labels_,
                          re);

        std::vector<std::pair<int, double>> const params
        {
            {64,4.5},{64,4.25},{64,4},{64,3.75},
            //{64,3.5},{64,3.25},{64,3},{64,2.75},{64,2.5},{64,2.25},
            //{64,2},{64,1.75},{64,1.5},{64,1.25},{64,1.0},{64,0.75},
            //{64,0.75}
        };
        for(size_t i = 0; i != params.size(); ++i){
            NetType nn;
            create_arch_01(nn);
            auto const pair = params[i];
            size_t const minibatch_size = pair.first;
            constexpr int num_epochs = 10;
            auto const str = std::to_string(pair.first) +
                    "_" + std::to_string(pair.second);
            //nn.optimizer().alpha *= std::sqrt(minibatch_size);
            nn.optimizer().alpha *= pair.second;
            ocv::tiny_cnn::trainer tt("car_weights_" + str,
                                      minibatch_size, num_epochs);
            std::ofstream out("net_" + str + ".txt");
            tt.train_and_test(nn, train_images_, train_labels_,
                              test_images_, test_labels_,
                              out);
        }//*/
    }catch(std::exception const &ex){
        std::cout<<ex.what()<<std::endl;
    }
}
