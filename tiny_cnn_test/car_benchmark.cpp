#include "car_benchmark.hpp"

#include <ocv_libs/core/utility.hpp>
#include <ocv_libs/file/utility.hpp>
#include <ocv_libs/ml/utility/shuffle_data.hpp>
#include <ocv_libs/tiny_cnn/image_converter.hpp>
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

constexpr cv::ImreadModes imread_modes = cv::IMREAD_GRAYSCALE;

template<typename Net>
void create_lenet(Net &nn)
{
    using activate = relu;

    //lenet 0
    //1913/2000, c1, 10 epoch, 20 batch, sqrt(20) alpha, relu, network<mse, adagrad>
    //1941/2000, c1, 12 epoch, 20 batch, 3 alpha, relu, network<mse, adagrad>, 6000, augment h/v
    //1950/2000, c1, 12 epoch, 20 batch, 3 alpha, relu, network<mse, adagrad>, 6000, augment h/v
    //1944/2000, c3, 12 epoch, 20 batch, 3 alpha, relu, network<mse, adagrad>, 6000, augment h/v
    //1955/2000, c1, 10 epoch, 15 batch, 2 alpha, relu, network<mse, adagrad>, 6000, augment h/v
    //1956/2000, c1, 12 epoch, 15 batch, 2 alpha, relu, network<mse, adagrad>, 6000, augment h/v
    //1951/2000, c1, 10 epoch, 15 batch, 2 alpha, relu, network<mse, adagrad>, 6000, augment h/v/illu
    //1949/2000, c1, 10 epoch, 15 batch, 2 alpha, relu, network<mse, adagrad>, 6000, augment h/v/illu/v+h
    //1949/2000, c1, 10 epoch, 12 batch, 1.8 alpha, relu, network<mse, adagrad>, 6000, augment h/v/illu
    //1938/2000, c1, 10 epoch, 15 batch, 2 alpha, relu, network<mse, adagrad>, 8144, augment h/v
    //1930/2000, c1, 10 epoch, 20 batch, 3 alpha, relu, network<mse, adagrad>, 8144, augment h/v
    //1925/2000, c1, 10 epoch, 10 batch, 2 alpha, relu, network<mse, adagrad>, 8144, augment h/v
    //1912/2000, c1, 5 epoch, 5 batch, 0.7 alpha, relu, network<mse, adam>, 8144, augment h/v
    //1892/2000, c1, 5 epoch, 5 batch, 0.7 alpha, relu, network<mse, adagrad>, 8144, augment h/v
    //1922/2000, c3, 5 epoch, 10 batch, 1.5 alpha, relu, network<mse, adagrad>, 8144, augment h/v

    //1915/2000, c1, 10 epoch, 32 batch, 2.25 alpha, relu, network<mse, adagrad>, 8144, augment h/v/gn
    /*nn << convolutional_layer<activate>(32, 32, 3, 1, 12, padding::same)
       << max_pooling_layer<activate>(32, 32, 12, 2)
       << convolutional_layer<activate>(16, 16, 3, 12, 18, padding::same)
       << max_pooling_layer<activate>(16, 16, 18, 2)
       << convolutional_layer<activate>(8, 8, 5, 18, 24)
       << fully_connected_layer<softmax>(384, 2);//*/

    //lenet 1
    //1927/2000, c1, 15 epoch, 20 batch, sqrt(20) alpha, relu, network<mse, adagrad>
    //1000/2000, c1, 10 epoch, 20 batch, sqrt(20) alpha, relu, network<mse, adam>, 6000, augment h/v
    //1934/2000, c1, 12 epoch, 20 batch, 3 alpha, relu, network<mse, adagrad>, 6000, augment h/v
    //1938/2000, c1, 12 epoch, 20 batch, 3 alpha, relu, network<mse, adagrad>, 6000, augment h/v
    //1932/2000, c1, 12 epoch, 20 batch, 3 alpha, elu, network<mse, adagrad>, 6000, augment h/v
    //1933/2000, c1, 15 epoch, 20 batch, 3 alpha, elu, network<mse, adagrad>, 6000, augment h/v
    //1936/2000, c1, 18 epoch, 20 batch, 3 alpha, elu, network<mse, adagrad>, 6000, augment h/v
    //1929/2000, c1, 12 epoch, 20 batch, 3 alpha, leaky_relu, network<mse, adagrad>, 6000, augment h/v
    //1936/2000, c1, 12 epoch, 20 batch, 3 alpha, relu, network<mse, adagrad>, 6000, augment h/v/(h+v)
    //1906/2000, c1, 12 epoch, 20 batch, 3 alpha, relu, network<mse, adagrad>, 6000, augment h/v
    //1907/2000, c1, 10 epoch, 20 batch, 2 alpha, relu, network<mse, adagrad>, 8144, augment h/v
    /*nn << convolutional_layer<activate>(32, 32, 3, 1, 10, padding::same)
       << max_pooling_layer<activate>(32, 32, 10, 2)
       << convolutional_layer<activate>(16, 16, 3, 10, 16, padding::same)
       << max_pooling_layer<activate>(16, 16, 16, 2)
       << convolutional_layer<activate>(8, 8, 5, 16, 24)
       << fully_connected_layer<softmax>(4*4*24, 2);//*/

    //lenet 2
    //1898/2000, c1, 12 epoch, 20 batch, 3 alpha, relu, network<mse, adagrad>, 6000, augment h/v
    //1885/2000, c1, 12 epoch, 20 batch, 3 alpha, relu, network<mse, adagrad>, 6000, augment h/v
    /*nn << convolutional_layer<activate>(32, 32, 3, 1, 10, padding::same)
       << max_pooling_layer<activate>(32, 32, 10, 3, 2)
       << convolutional_layer<activate>(16, 16, 3, 10, 16, padding::same)
       << max_pooling_layer<activate>(16, 16, 16, 3, 2)
       << convolutional_layer<activate>(8, 8, 5, 16, 24)
       << fully_connected_layer<softmax>(4*4*24, 2);//*/

    //lenet 3
    /*nn << convolutional_layer<activate>(32, 32, 3, 1, 10, padding::same)
       << max_pooling_layer<activate>(32, 32, 10, 2)
       << convolutional_layer<activate>(16, 16, 3, 10, 16, padding::same)
       << max_pooling_layer<activate>(16, 16, 16, 2)
       << convolutional_layer<activate>(8, 8, 5, 16, 24, padding::same)
       << max_pooling_layer<activate>(8, 8, 24, 2)
       << fully_connected_layer<identity>(4*4*24, 4*4*24)
       << fully_connected_layer<softmax>(4*4*24, 2);//*/

    //lenet 4
    //1952/2000, c1, 10 epoch, 20 batch, 3 alpha, relu, network<mse, adagrad>, 6000, augment h/v
    //1917/2000, c1, 5 epoch, 10 batch, 1.5 alpha, relu, network<mse, adagrad>, 8144, augment h/v
    /*nn << convolutional_layer<activate>(32, 32, 3, 1, 12, padding::same)
       << max_pooling_layer<activate>(32, 32, 12, 2)
       << convolutional_layer<activate>(16, 16, 3, 12, 18, padding::same)
       << max_pooling_layer<activate>(16, 16, 18, 2)
       << convolutional_layer<activate>(8, 8, 5, 18, 24, padding::same)
       << max_pooling_layer<activate>(8, 8, 24, 2)
       << convolutional_layer<activate>(4, 4, 3, 24, 32, padding::same)
       << fully_connected_layer<softmax>(4*4*32, 2);//*/

    //lenet 5
    //1948/2000, c1, 10 epoch, 20 batch, 3 alpha, relu, network<mse, adagrad>, 6000, augment h/v
    //1917/2000, c1, 5 epoch, 10 batch, 1.5 alpha, relu, network<mse, adagrad>, 8144, augment h/v
    /*nn << convolutional_layer<activate>(32, 32, 3, 1, 12, padding::same)
       << max_pooling_layer<activate>(32, 32, 12, 2)
       << convolutional_layer<activate>(16, 16, 3, 12, 18, padding::same)
       << max_pooling_layer<activate>(16, 16, 18, 2)
       << convolutional_layer<activate>(8, 8, 5, 18, 24, padding::same)
       << max_pooling_layer<activate>(8, 8, 24, 2)
       << fully_connected_layer<activate>(4*4*24, 4*4*24)
       << dropout_layer(4*4*24, 0.5)
       << fully_connected_layer<softmax>(4*4*24, 2);//*/

    //lenet 6
    //1578/2000, c1, 10 epoch, 20 batch, 3 alpha, relu, network<mse, adagrad>, 6000, augment h/v
    /*nn << convolutional_layer<activate>(32, 32, 3, 1, 12, padding::same)
       << max_pooling_layer<activate>(32, 32, 12, 2)
       << convolutional_layer<activate>(16, 16, 3, 12, 18, padding::same)
       << max_pooling_layer<activate>(16, 16, 18, 2)
       << convolutional_layer<activate>(8, 8, 5, 18, 24, padding::same)
       << max_pooling_layer<activate>(8, 8, 24, 2)
       << dropout_layer(4*4*24, 0.5)
       << fully_connected_layer<activate>(4*4*24, 4*4*24)
       << fully_connected_layer<softmax>(4*4*24, 2);//*/

    //lenet 7
    //1913/2000, c1, 10 epoch, 20 batch, 3 alpha, relu, network<mse, adagrad>, 6000, augment h/v
    //1915/2000, c1, 10 epoch, 15 batch, 2.5 alpha, relu, network<mse, adagrad>, 6000, augment h/v
    /*nn << convolutional_layer<activate>(32, 32, 3, 1, 14, padding::same)
       << max_pooling_layer<activate>(32, 32, 14, 2)
       << convolutional_layer<activate>(16, 16, 3, 14, 20, padding::same)
       << max_pooling_layer<activate>(16, 16, 20, 2)
       << convolutional_layer<activate>(8, 8, 5, 20, 26)
       << fully_connected_layer<softmax>(4*4*26, 2);//*/

    //lenet 8
    //1825/2000, c1, 10 epoch, 15 batch, 2.5 alpha, relu, network<mse, adam>, 6000, augment h/v
    /*nn << convolutional_layer<activate>(32, 32, 3, 1, 14, padding::same)
       << max_pooling_layer<activate>(32, 32, 14, 2)
       << dropout_layer(16*16*14, 0.25)
       << convolutional_layer<activate>(16, 16, 3, 14, 20, padding::same)
       << max_pooling_layer<activate>(16, 16, 20, 2)
       << convolutional_layer<activate>(8, 8, 5, 20, 26)
       << fully_connected_layer<softmax>(4*4*26, 2);//*/

    //lenet 9
    //1000/2000, c1, 10 epoch, 20 batch, 3 alpha, relu, network<mse, adam>, 6000, augment h/v
    /*nn << convolutional_layer<activate>(32, 32, 3, 1, 14, padding::same)
       << max_pooling_layer<activate>(32, 32, 14, 2)
       << dropout_layer(16*16*14, 0.25)
       << convolutional_layer<activate>(16, 16, 3, 14, 20, padding::same)
       << max_pooling_layer<activate>(16, 16, 20, 2)
       << dropout_layer(8*8*20, 0.25)
       << convolutional_layer<activate>(8, 8, 5, 20, 26, padding::same)
       << max_pooling_layer<activate>(8, 8, 26, 2)
       << fully_connected_layer<activate>(4*4*26, 4*4*26)
       << dropout_layer(4*4*26, 0.5)
       << fully_connected_layer<softmax>(4*4*26, 2);//*/

    //lenet 10
    //1946/2000, c1, 10 epoch, 20 batch, 3 alpha, relu, network<mse, adagrad>, 6000, augment h/v
    //1956/2000, c1, 10 epoch, 20 batch, 2.5 alpha, relu, network<mse, adagrad>, 6000, augment h/v
    //1946/2000, c1, 10 epoch, 20 batch, 2.5 alpha, relu, network<mse, RMSProp>, 6000, augment h/v
    //1930/2000, c1, 10 epoch, 20 batch, 3 alpha, relu, network<mse, RMSProp>, 6000, augment h/v
    //1949/2000, c1, 10 epoch, 20 batch, 2 alpha, relu, network<mse, adagrad>, 6000, augment h/v
    //1925/2000, c1, 10 epoch, 1 batch, 1.5 alpha, relu, network<mse, adagrad>, 8144, augment h/v
    //1930/2000, c1, 10 epoch, 5 batch, 1.5 alpha, relu, network<mse, adagrad>, 8144, augment h/v
    //1928/2000, c1, 10 epoch, 10 batch, 1.5 alpha, relu, network<mse, adagrad>, 8144, augment h/v

    //1841/2000, c1, 5 epoch, 32 batch, 0.25 alpha, relu, network<mse, adagrad>, 8144, augment h/v
    //1813/2000, c1, 5 epoch, 32 batch, 0.5 alpha, relu, network<mse, adagrad>, 8144, augment h/v
    //1826/2000, c1, 5 epoch, 32 batch, 0.75 alpha, relu, network<mse, adagrad>, 8144, augment h/v
    //1789/2000, c1, 5 epoch, 32 batch, 1 alpha, relu, network<mse, adagrad>, 8144, augment h/v
    //1830/2000, c1, 5 epoch, 32 batch, 1.25 alpha, relu, network<mse, adagrad>, 8144, augment h/v
    //1853/2000, c1, 5 epoch, 32 batch, 1.5 alpha, relu, network<mse, adagrad>, 8144, augment h/v
    //1914/2000, c1, 5 epoch, 32 batch, 2.25 alpha, relu, network<mse, adagrad>, 8144, augment h/v
    //1933/2000, c1, 10 epoch, 32 batch, 2.25 alpha, relu, network<mse, adagrad>, 8144, augment h/v

    //1913/2000, c1, 5 epoch, 128 batch, 2 alpha, relu, network<mse, adagrad>, 8144, augment h/v
    nn << convolutional_layer<activate>(32, 32, 3, 1, 12, padding::same)
       << max_pooling_layer<activate>(32, 32, 12, 2)
       << convolutional_layer<activate>(16, 16, 3, 12, 18, padding::same)
       << max_pooling_layer<activate>(16, 16, 18, 2)
       << convolutional_layer<activate>(8, 8, 5, 18, 24, padding::same)
       << max_pooling_layer<activate>(8, 8, 24, 2)
       << convolutional_layer<activate>(4, 4, 3, 24, 32, padding::same)
       << max_pooling_layer<activate>(4, 4, 32, 2)
       << fully_connected_layer<softmax>(2*2*32, 2);//*/

    //lenet 11
    //1949/2000, c1, 10 epoch, 20 batch, 2 alpha, relu, network<mse, adagrad>, 6000, augment h/v
    //1921/2000, c1, 10 epoch, 20 batch, 3 alpha, relu, network<mse, adagrad>, 6000, augment h/v
    //1956/2000, c1, 10 epoch, 15 batch, 2 alpha, relu, network<mse, adagrad>, 6000, augment h/v
    //1944/2000, c1, 12 epoch, 15 batch, 2 alpha, relu, network<mse, adagrad>, 6000, augment h/v
    //1942/2000, c1, 10 epoch, 15 batch, 2 alpha, relu, network<mse, adagrad>, 6000, augment h/v/illu
    /*nn << convolutional_layer<activate>(32, 32, 3, 1, 12, padding::same)
       << max_pooling_layer<activate>(32, 32, 12, 2)
       << convolutional_layer<activate>(16, 16, 3, 12, 18, padding::same)
       << max_pooling_layer<activate>(16, 16, 18, 2)
       << convolutional_layer<activate>(8, 8, 5, 18, 24, padding::same)
       << max_pooling_layer<activate>(8, 8, 24, 2)
       << convolutional_layer<activate>(4, 4, 3, 24, 32, padding::same)
       << max_pooling_layer<activate>(4, 4, 32, 2)
       << convolutional_layer<activate>(2, 2, 3, 32, 36, padding::same)
       << fully_connected_layer<softmax>(2*2*36, 2);//*/

    //lenet 12
    //1940/2000, c1, 10 epoch, 15 batch, 2 alpha, relu, network<mse, adagrad>, 6000, augment h/v
    /*nn << convolutional_layer<activate>(32, 32, 3, 1, 12, padding::same)
       << max_pooling_layer<activate>(32, 32, 12, 2)
       << convolutional_layer<activate>(16, 16, 3, 12, 18, padding::same)
       << max_pooling_layer<activate>(16, 16, 18, 2)
       << convolutional_layer<activate>(8, 8, 5, 18, 24, padding::same)
       << max_pooling_layer<activate>(8, 8, 24, 2)
       << convolutional_layer<activate>(4, 4, 3, 24, 32, padding::same)
       << max_pooling_layer<activate>(4, 4, 32, 2)
       << dropout_layer(2*2*32, 0.5)
       << convolutional_layer<activate>(2, 2, 3, 32, 36, padding::same)
       << fully_connected_layer<softmax>(2*2*36, 2);//*/

    //lenet 13
    //1898/2000, c1, 5 epoch, 15 batch, 1.5 alpha, relu, network<mse, adam>, 6000, augment h/v
    //1944/2000, c1, 5 epoch, 15 batch, 2 alpha, relu, network<mse, adam>, 8144, augment h/v
    /*nn << convolutional_layer<activate>(64, 64, 3, 1, 12, padding::same)
       << max_pooling_layer<activate>(64, 64, 12, 2)
       << convolutional_layer<activate>(32, 32, 3, 12, 18, padding::same)
       << max_pooling_layer<activate>(32, 32, 18, 2)
       << convolutional_layer<activate>(16, 16, 5, 18, 24)
       << fully_connected_layer<softmax>(12*12*24, 2);//*/

    //lenet 14
    //1903/2000, c1, 5 epoch, 15 batch, 2 alpha, relu, network<mse, adagrad>, 8144, augment h/v
    //1915/2000, c1, 5 epoch, 10 batch, 1.5 alpha, relu, network<mse, adagrad>, 8144, augment h/v
    /*nn << convolutional_layer<activate>(64, 64, 3, 1, 12, padding::same)
       << max_pooling_layer<activate>(64, 64, 12, 2)
       << convolutional_layer<activate>(32, 32, 3, 12, 18, padding::same)
       << max_pooling_layer<activate>(32, 32, 18, 2)
       << convolutional_layer<activate>(16, 16, 5, 18, 24, padding::same)
       << max_pooling_layer<activate>(16, 16, 24, 2)
       << convolutional_layer<activate>(8, 8, 3, 24, 32, padding::same)
       << max_pooling_layer<activate>(8, 8, 32, 2)
       << dropout_layer(4*4*32, 0.5)
       << convolutional_layer<activate>(4, 4, 3, 32, 36, padding::same)
       << fully_connected_layer<softmax>(4*4*36, 2);//*/

    //lenet 15
    //1924/2000, c1, 5 epoch, 1 batch, 1.5 alpha, relu, network<mse, adagrad>, 8144, augment h/v
    //1917/2000, c1, 5 epoch, 5 batch, 1.5 alpha, relu, network<mse, adagrad>, 8144, augment h/v
    /*nn << convolutional_layer<activate>(32, 32, 3, 1, 12, padding::same)
       << max_pooling_layer<activate>(32, 32, 12, 2)
       << convolutional_layer<activate>(16, 16, 3, 12, 18, padding::same)
       << max_pooling_layer<activate>(16, 16, 18, 2)
       << convolutional_layer<activate>(8, 8, 5, 18, 24, padding::same)
       << fully_connected_layer<softmax>(8*8*24, 2);//*/
}

template<typename Net>
void create_minivgg(Net &nn)
{
    using activate = relu;

    //mvgg0
    //1550/2000, 10 epoch, 15 batch, 1 alpha, relu, net<mse,adam>, 6000, augment horizontal,vertical
    /*nn << convolutional_layer<activate>(32, 32, 3, 1, 32, padding::same)
       << convolutional_layer<activate>(32, 32, 3, 32, 32, padding::same)
       << max_pooling_layer<activate>(32, 32, 32, 2)
       << dropout_layer(16*16*32, 0.25)
       << convolutional_layer<activate>(16, 16, 3, 32, 64, padding::same)
       << convolutional_layer<activate>(16, 16, 3, 64, 64, padding::same)
       << max_pooling_layer<activate>(16, 16, 64, 2)
       << dropout_layer(8*8*64, 0.25)
       << fully_connected_layer<softmax>(8*8*64, 2);//*/

    //mvgg1
    //1648/2000, c1, 5 epoch, 20 batch, 3 alpha, relu, network<mse, adagrad>, 6000, augment h/v
    /*nn << convolutional_layer<activate>(32, 32, 3, 1, 8, padding::same)
       << dropout_layer(32*32*8, 0.25)
       << convolutional_layer<activate>(32, 32, 3, 8, 16, padding::same)
       << dropout_layer(32*32*16, 0.25)
       << convolutional_layer<activate>(32, 32, 3, 16, 24, padding::same)
       << dropout_layer(32*32*24, 0.5)
       << fully_connected_layer<softmax>(32*32*24, 2);//*/

    //mvgg1
    //1752/2000, c1, 5 epoch, 20 batch, 3 alpha, relu, network<mse, adagrad>, 6000, augment h/v
    /*nn << convolutional_layer<activate>(32, 32, 3, 1, 8, padding::same)
       << convolutional_layer<activate>(32, 32, 3, 8, 8, padding::same)
       << max_pooling_layer<activate>(32, 32, 8, 2)
       << dropout_layer(16*16*8, 0.25)
       << convolutional_layer<activate>(16, 16, 3, 8, 16, padding::same)
       << convolutional_layer<activate>(16, 16, 3, 16, 16, padding::same)
       << max_pooling_layer<activate>(16, 16, 16, 2)
       << dropout_layer(8*8*16, 0.25)
       << fully_connected_layer<softmax>(8*8*16, 2);//*/

    //mvgg2
    //1926/2000, c1, 5 epoch, 20 batch, 3 alpha, relu, network<mse, adagrad>, 6000, augment h/v
    /*nn << convolutional_layer<activate>(32, 32, 3, 1, 8, padding::same)
       << convolutional_layer<activate>(32, 32, 3, 8, 8, padding::same)
       << max_pooling_layer<activate>(32, 32, 8, 2)
       << convolutional_layer<activate>(16, 16, 3, 8, 16, padding::same)
       << convolutional_layer<activate>(16, 16, 3, 16, 16, padding::same)
       << max_pooling_layer<activate>(16, 16, 16, 2)
       //<< fully_connected_layer<relu>(8*8*16, 8*8*16)
       << fully_connected_layer<softmax>(8*8*16, 2);//*/

    //mvgg3
    //1888/2000, c1, 5 epoch, 20 batch, 3 alpha, relu, network<mse, adagrad>, 6000, augment h/v
    /*nn << convolutional_layer<activate>(32, 32, 3, 1, 8, padding::same)
       << convolutional_layer<activate>(32, 32, 3, 8, 8, padding::same)
       << dropout_layer(32*32*8, 0.25)
       << max_pooling_layer<activate>(32, 32, 8, 2)
       << convolutional_layer<activate>(16, 16, 3, 8, 16, padding::same)
       << convolutional_layer<activate>(16, 16, 3, 16, 16, padding::same)
       << max_pooling_layer<activate>(16, 16, 16, 2)
       //<< fully_connected_layer<relu>(8*8*16, 8*8*16)
       << fully_connected_layer<softmax>(8*8*16, 2);//*/

    //mvgg4
    //1910/2000, c1, 5 epoch, 20 batch, 3 alpha, relu, network<mse, adam>, 6000, augment h/v
    /*nn << convolutional_layer<activate>(32, 32, 3, 1, 8, padding::same)
       << convolutional_layer<activate>(32, 32, 3, 8, 8, padding::same)
       << max_pooling_layer<activate>(32, 32, 8, 2)
       << convolutional_layer<activate>(16, 16, 3, 8, 16, padding::same)
       << convolutional_layer<activate>(16, 16, 3, 16, 16, padding::same)
       << max_pooling_layer<activate>(16, 16, 16, 2)
       << fully_connected_layer<relu>(8*8*16, 8*8*16)
       << fully_connected_layer<softmax>(8*8*16, 2);//*/

    //mvgg5
    //1840/2000, c1, 5 epoch, 15 batch, 2 alpha, relu, network<mse, adagrad>, 8144, augment h/v
    nn << convolutional_layer<activate>(64, 64, 3, 1, 8, padding::same)
       << convolutional_layer<activate>(64, 64, 3, 8, 8, padding::same)
       << max_pooling_layer<activate>(64, 64, 8, 2)
       << dropout_layer(32*32*8, 0.25)
       << convolutional_layer<activate>(32, 32, 3, 8, 16, padding::same)
       << convolutional_layer<activate>(32, 32, 3, 16, 16, padding::same)
       << max_pooling_layer<activate>(32, 32, 16, 2)
       << dropout_layer(16*16*16, 0.5)
       << fully_connected_layer<softmax>(16*16*16, 2);//*/
}

}

void car_benchmark::load_data()
{
    constexpr bool train_augment = true;
    load_car_data("train_cars", load_car_region("cars_train_annos.txt"),
                  train_augment, train_images_, train_labels_);
    load_car_data("test_cars", load_car_region("cars_test_annos.txt"),
                  false, test_images_, test_labels_);

    load_data("train_non_cars", 1, train_augment,
              train_images_, train_labels_);
    load_data("test_non_cars", 1, false,
              test_images_, test_labels_);//*/

    /*parse_cifar10("cifar-10-batches-bin/data_batch_1.bin", &train_images_, &train_labels_, 0, 1, 0, 0);
    parse_cifar10("cifar-10-batches-bin/data_batch_2.bin", &train_images_, &train_labels_, 0, 1, 0, 0);
    parse_cifar10("cifar-10-batches-bin/data_batch_3.bin", &train_images_, &train_labels_, 0, 1, 0, 0);
    parse_cifar10("cifar-10-batches-bin/data_batch_4.bin", &train_images_, &train_labels_, 0, 1, 0, 0);
    parse_cifar10("cifar-10-batches-bin/data_batch_5.bin", &train_images_, &train_labels_, 0, 1, 0, 0);
    parse_cifar10("cifar-10-batches-bin/test_batch.bin", &test_images_, &test_labels_, 0, 1, 0, 0);//*/

    ocv::ml::shuffles(train_images_, train_labels_);
}

car_benchmark::car_benchmark()
{                
    //create_lenet(nn_);
    //std::ifstream in("car_weights_lenet0_c1_1956_2000");
    //in>>nn_;
}

std::vector<std::pair<double, int> >
car_benchmark::predict(const cv::Mat &input)
{
    using namespace ocv::tiny_cnn;

    cv::Mat resize;
    cv::resize(input, resize, {32,32});
    auto const vec = cvmat_to_img<vec_t>(resize);

    auto const prob = nn_.predict(vec);
    std::vector<std::pair<double, int>> result;
    for(size_t i = 0; i != prob.size(); ++i){
        result.emplace_back(prob[i], i);
    }

    return result;
}

void car_benchmark::add_data(label_t label, cv::Mat const &img,
                             bool augment, Labels &labels,
                             Imgs &imgs)
{
    using namespace ocv::tiny_cnn;
    if(!img.empty()){
        constexpr float min = -1;
        constexpr float max = 1;
        cv::Mat resize_img;
        cv::resize(img, resize_img, {32,32});
        imgs.emplace_back(cvmat_to_img<vec_t>(resize_img, min, max));
        labels.emplace_back(label);

        if(augment){
            cv::Mat origin = resize_img.clone();

            cv::flip(origin, resize_img, 0); //flip horizontal
            imgs.emplace_back(cvmat_to_img<vec_t>(resize_img, min, max));
            labels.emplace_back(label);

            cv::flip(origin, resize_img, 1); //flip vertical
            imgs.emplace_back(cvmat_to_img<vec_t>(resize_img, min, max));
            labels.emplace_back(label);

            //add gaussian noise
            static std::random_device rd;
            static std::mt19937 generator(rd());
            constexpr double mean = 0.0;
            constexpr double stddev  = 30;
            static std::normal_distribution<double> normal(mean, stddev);
            origin.copyTo(resize_img);

            ocv::for_each_channels<uchar>(resize_img, [&](uchar &val)
            {
                val =  cv::saturate_cast<uchar>(val + normal(generator));
            });//*/

            imgs.emplace_back(cvmat_to_img<vec_t>(resize_img, min, max));
            labels.emplace_back(label);

            //cv::flip(origin, resize_img, -1); //flip vertical and horizontal
            //imgs.emplace_back(cvmat_to_img<vec_t>(resize_img, min, max));
            //labels.emplace_back(label);

            /*static std::random_device rd;
            static std::mt19937 gen(rd());
            static std::uniform_int_distribution<int> uid(-20, -10);
            resize_img = origin + uid(gen);
            imgs.emplace_back(cvmat_to_img<vec_t>(resize_img, min, max));
            labels.emplace_back(label);//*/
        }
    }
}

void car_benchmark::load_car_data(const std::string &folder,
                                  std::map<std::string, cv::Rect> const &car_region,
                                  bool augment, Imgs &imgs,
                                  Labels &labels)
{
    auto const names = ocv::file::get_directory_files(folder);
    constexpr int expand_pix = 16;
    for(auto const &name : names){
        cv::Mat img = cv::imread(folder + "/" + name,
                                 imread_modes);
        auto const rect =
                ocv::expand_region(img.size(), car_region.at(name),
                                   expand_pix);

        add_data(0, img(rect), augment, labels, imgs);
    }
}

std::map<std::string, cv::Rect> car_benchmark::
load_car_region(const std::string &file) const
{
    std::ifstream in(file);
    int x1,x2,y1,y2;
    std::string name;
    std::map<std::string, cv::Rect> result;
    while(!in.eof()){
        in>>x1>>x2>>y1>>y2>>name;
        result.insert({name, {x1, y1, x2-x1, y2-y1}});
    }

    return result;
}

void car_benchmark::load_data(std::string const &folder,
                              label_t label,
                              bool augment,
                              Imgs &imgs, Labels &labels)
{
    auto const names = ocv::file::get_directory_files(folder);
    for(auto const &name : names){
        cv::Mat img = cv::imread(folder + "/" + name,
                                 imread_modes);
        add_data(label, img, augment, labels, imgs);
    }
}

void car_benchmark::train()
{    
    try{
        load_data();

        network<mse, adagrad> nn;
        create_lenet(nn);

        std::vector<std::pair<int, double>> const params
        {
            //{32,0.25},{32,0.5},{32,0.75},{32,1},{32,1.25},{32,1.5},
            //{128,2},{128,2.5},{128,2.75},{128,3},{128,3.25},{128,3.5},
            {32,2.25}
        };
        for(size_t i = 0; i != params.size(); ++i){
            int const minibatch_size = params[i].first;
            constexpr int num_epochs = 10;

            //nn.optimizer().alpha *= std::sqrt(minibatch_size);
            nn.optimizer().alpha *= params[i].second;
            ocv::tiny_cnn::trainer tt("car_weights_" + std::to_string(i),
                                      minibatch_size, num_epochs);
            std::ofstream out("net_" + std::to_string(i) + ".txt");
            tt.train_and_test(nn, train_images_, train_labels_,
                              test_images_, test_labels_,
                              out);
        }
    }catch(std::exception const &ex){
        std::cout<<ex.what()<<std::endl;
    }


}
