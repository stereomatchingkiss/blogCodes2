#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <bugLocation.hpp>

#include "zhanSuen.hpp"

namespace
{

/**
 * Perform one thinning iteration.
 * Normally you wouldn't call this function directly from your code.
 *
 * @param  input Binary image with range = 0-1
 * @param  iter  0=even, 1=odd
 */
/**
 * Perform one thinning iteration.
 * Normally you wouldn't call this function directly from your code.
 *
 * @param  input Binary image with range = 0-1
 * @param  iter  0=even, 1=odd
 */
void thinning_iteration(cv::Mat& input, int iter)
{
    cv::Mat marker = cv::Mat::zeros(input.size(), CV_8UC1);

    for (int i = 1; i < input.rows-1; ++i){
        auto previous_row = input.ptr<uchar>(i - 1);
        auto current_row = input.ptr<uchar>(i);
        auto next_row = input.ptr<uchar>(i + 1);
        auto marker_ptr = marker.ptr<uchar>(i);
        for (int j = 1; j < input.cols - 1; ++j){
            uchar p2 = previous_row[j]; //input.at<uchar>(i-1, j);
            uchar p3 = previous_row[j + 1]; //input.at<uchar>(i-1, j+1);
            uchar p4 = current_row[j + 1]; //input.at<uchar>(i, j+1);
            uchar p5 = next_row[j + 1]; //input.at<uchar>(i+1, j+1);
            uchar p6 = next_row[j];  //input.at<uchar>(i+1, j);
            uchar p7 = next_row[j - 1]; //input.at<uchar>(i+1, j-1);
            uchar p8 = current_row[j - 1]; //input.at<uchar>(i, j-1);
            uchar p9 = previous_row[j - 1]; //input.at<uchar>(i-1, j-1);

            int A  = (p2 == 0 && p3 == 1) + (p3 == 0 && p4 == 1) +
                    (p4 == 0 && p5 == 1) + (p5 == 0 && p6 == 1) +
                    (p6 == 0 && p7 == 1) + (p7 == 0 && p8 == 1) +
                    (p8 == 0 && p9 == 1) + (p9 == 0 && p2 == 1);
            int B  = p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9;
            int m1 = iter == 0 ? (p2 * p4 * p6) : (p2 * p4 * p8);
            int m2 = iter == 0 ? (p4 * p6 * p8) : (p2 * p6 * p8);

            if (A == 1 && (B >= 2 && B <= 6) && m1 == 0 && m2 == 0){
                marker_ptr[j] = 1;
            }
        }
    }

    //input &= ~marker;
    input -= marker;
}

}

namespace OCV
{

/**
 * Function for thinning the given binary image
 *
 * @param  input  Binary image with values = 0, 255
 */
void zhan_suen_thinning(cv::Mat &input, cv::Mat &output)
{
    if(input.type() != CV_8U){
        throw std::runtime_error(COMMON_DEBUG_MESSAGE + "input.type() != CV_8U");
    }

    cv::Mat prev = cv::Mat::zeros(input.size(), CV_8UC1);
    if(input.data != output.data){
        output = input.clone();
    }
    cv::Mat diff;
    output /= 255;

    do {
        thinning_iteration(output, 0);
        thinning_iteration(output, 1);
        cv::absdiff(output, prev, diff);
        output.copyTo(prev);
    }while (cv::countNonZero(diff) > 0);

    output *= 255;
}

}
