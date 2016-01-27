#include "car_benchmark.hpp"
#include "utility.hpp"

#include <boost/progress.hpp>

#include <tiny_cnn/tiny_cnn.h>

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
}
