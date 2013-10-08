#include <stdexcept>
#include <string>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <bugLocation.hpp>

/**
 * @brief implement   harris corners, these codes are design for learning purpose but not for real case
 * @param input       Input single-channel 8-bit or floating-point image.
 * @param output      Image to store the Harris detector responses. It has the type CV_32FC1 and the same size as src .
 * @param blockSize   Neighborhood size
 * @param ksize       Aperture parameter for the Sobel() operator.
 * @param k           Harris detector free parameter.
 */
void harris_corners(cv::Mat const &input, cv::Mat &output, int block_size, int ksize, float k)
{
    //step 1 : make sure the type of the input is valid
    if(!(input.type() == CV_8U || input.type() == CV_32F)){
        throw std::runtime_error(COMMON_DEBUG_MESSAGE +
                                 "input.type() != CV_8U || input.type() != CV_32F\n");
    }

    output.create(input.size(), CV_32F);

    //step 2 : scale need to normalize the result of the sobel filter
    //I don't get it why they initialize the scale like this
    //if anyone figure it out please tell me why, thanks
    double scale = (double)(1 << ((ksize > 0 ? ksize : 3) - 1)) * block_size;

    //the range of CV_8U are in range [0, 255];
    //CV_32F are in range [0, 1]
    if(input.type() == CV_8U){
        scale *= 255.0;
    }
    scale = 1.0/scale;

    //step 3 : calculate the gradient of x and y direction
    cv::Mat dx, dy;
    cv::Sobel(input, dx, CV_32F, 1, 0, ksize, scale);
    cv::Sobel(input, dy, CV_32F, 0, 1, ksize, scale);

    cv::Size size = input.size();
    //step 4 : convolution and save dx*dx, dx*dy, dy*dy in cov
    //I prefer auto because the types of dx, dy and cov may
    //changed in the future
    using Type = float;
    cv::Mat cov(size, CV_32FC3);
    for(int i = 0; i < size.height; ++i){
        Type *cov_data = cov.ptr<Type>(i);
        Type const *dxdata = dx.ptr<Type>(i);
        Type const *dydata = dy.ptr<Type>(i);

        for(int j = 0; j < size.width; ++j){
            Type const dx = dxdata[j];
            Type const dy = dydata[j];
            *cov_data = dx*dx; ++cov_data;
            *cov_data = dx*dy; ++cov_data;
            *cov_data = dy*dy; ++cov_data;
        }
    }

    //step 5 : generate the result of Harris matrix, all of the weight are set as 1
    cv::boxFilter(cov, cov, cov.depth(), cv::Size(block_size, block_size), cv::Point(-1,-1), false);

    //step 6 : optimize the iteration speed, not a neccessary step for harris_corner
    if( cov.isContinuous() && output.isContinuous() ){
        size.width = input.total();
        size.height = 1;
    }

    //step 7 : find out Mc and save it to the output Mat
    for(int i = 0; i < size.height; ++i){
        Type const *cov_data = cov.ptr<Type>(i);
        Type *dst = output.ptr<float>(i);
        for(int j = 0; j < size.width; ++j){
            Type const a = *cov_data; ++cov_data;
            Type const b = *cov_data; ++cov_data;
            Type const c = *cov_data; ++cov_data;
            Type const temp = a + c;
            dst[j] = a*c - b*b - k*(temp)*(temp);
        }
    }
}
