#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/tracking.hpp>

#include <timeEstimate.hpp>

// Classical Hough Transform
struct LinePolar
{
    float rho;
    float angle;
};


struct hough_cmp_gt
{
    hough_cmp_gt(const int* _aux) : aux(_aux) {}
    bool operator()(int l1, int l2) const
    {
        return aux[l1] > aux[l2] || (aux[l1] == aux[l2] && l1 < l2);
    }
    const int* aux;
};

void
HoughLinesStandard_CV( const cv::Mat& img, float rho, float theta,
                     int threshold, std::vector<cv::Vec2f>& lines, int linesMax )
{
    int i, j; //#ra0
    float irho = 1 / rho;

    CV_Assert( img.type() == CV_8UC1 ); //#ra1

    const uchar* image = img.data; //#rb1
    int step = (int)img.step; //#rb2
    int width = img.cols; //#ra2
    int height = img.rows; //#ra3

    int numangle = cvRound(CV_PI / theta);
    int numrho = cvRound(((width + height) * 2 + 1) / rho); //#ra4, #1

    cv::AutoBuffer<int> _accum((numangle+2) * (numrho+2)); //#ra5
    std::vector<int> _sort_buf; //#ra6
    cv::AutoBuffer<float> _tabSin(numangle);
    cv::AutoBuffer<float> _tabCos(numangle);
    int *accum = _accum; //#ra7
    float *tabSin = _tabSin, *tabCos = _tabCos;

    memset( accum, 0, sizeof(accum[0]) * (numangle+2) * (numrho+2) );

    float ang = 0;
    for(int n = 0; n < numangle; ang += theta, n++ ) //#rd1
    {
        tabSin[n] = (float)(sin((double)ang) * irho); //#rc1
        tabCos[n] = (float)(cos((double)ang) * irho); //#rc2
    }

    // stage 1. fill accumulator
    for( i = 0; i < height; i++ ) //#rd2
        for( j = 0; j < width; j++ ) //#rd3
        {
            if( image[i * step + j] != 0 ) //#rb3
                for(int n = 0; n < numangle; n++ ) //#rd4
                {
                    int r = cvRound( j * tabCos[n] + i * tabSin[n] ); //#2a
                    r += (numrho - 1) / 2; //#2b
                    accum[(n+1) * (numrho+2) + r+1]++; //#rd5, #3
                }
        }

    // stage 2. find local maximums
    for(int r = 0; r < numrho; r++ ) //#rd6
        for(int n = 0; n < numangle; n++ ) //#rd7
        {
            int base = (n+1) * (numrho+2) + r+1;
            if( accum[base] > threshold &&
                    accum[base] > accum[base - 1] &&
                    accum[base] >= accum[base + 1] &&
                    accum[base] > accum[base - numrho - 2] &&
                    accum[base] >= accum[base + numrho + 2] )
                _sort_buf.push_back(base);
        }

    // stage 3. sort the detected lines by accumulator value
    std::sort(_sort_buf.begin(), _sort_buf.end(), hough_cmp_gt(accum));

    // stage 4. store the first min(total,linesMax) lines to the output buffer
    linesMax = std::min(linesMax, (int)_sort_buf.size()); //#re1
    double scale = 1./(numrho+2);
    for( i = 0; i < linesMax; i++ ) //#rd8
    {
        LinePolar line; //#ra8
        int idx = _sort_buf[i];
        int n = cvFloor(idx*scale) - 1; //#4
        int r = idx - (n+1)*(numrho+2) - 1; //#5
        line.rho = (r - (numrho - 1)*0.5f) * rho; //#6
        line.angle = n * theta;
        lines.push_back(cv::Vec2f(line.rho, line.angle)); //#rf1
    }
}


/*
Here image is an input raster;
step is it's step; size characterizes it's ROI;
rho and theta are discretization steps (in pixels and radians correspondingly).
threshold is the minimum number of pixels in the feature for it
to be a candidate for line. lines is the output
array of (rho, theta) pairs. linesMax is the buffer size (number of pairs).
Functions return the actual number of found lines.
*/
void HoughLinesStandard_RF( const cv::Mat& img, float rho, float theta,
                         int threshold, std::vector<cv::Vec2f>& lines, int linesMax )
{
    //check the type of img earlier, if
    //the type mismatched, don't need to do any
    //job(lazy evaluation)
    CV_Assert( img.type() == CV_8UC1 );

    float const irho = 1 / rho;
    int const numangle = cvRound(CV_PI / theta); //as rows of acc table
    cv::AutoBuffer<float> _tabSin(numangle);
    cv::AutoBuffer<float> _tabCos(numangle);
    float *tabSin = _tabSin, *tabCos = _tabCos;
    float ang = 0;
    for(int n = 0; n < numangle; ang += theta, ++n){
        //use std::sin to replace c-style sin, save the cost of casting from
        //float to double and double to float(cast two times)
        tabSin[n] = std::sin(ang) * irho;
        tabCos[n] = std::cos(ang) * irho;
    }

    // stage 1. fill accumulator
    int const width = img.cols;
    int const height = img.rows;
    int const numrho = cvRound(((width + height) * 2 + 1) / rho); //as columns of acc table
    cv::AutoBuffer<int> _accum((numangle + 2) * (numrho + 2));
    int *accum = _accum;
    memset( accum, 0, sizeof(accum[0]) * (numangle+2) * (numrho+2) );
    for(int i = 0; i < height; ++i){
        auto const img_ptr = img.ptr<uchar>(i);
        for(int j = 0; j < width; ++j){
            //access the pixel with cheaper operation
            //original one need (i * img.step + j)
            if( img_ptr[j] != 0 ){
                for(int n = 0; n < numangle; ++n){
                    int r = cvRound( j * tabCos[n] + i * tabSin[n] );
                    r += (numrho - 1) / 2;
                    ++accum[(n+1) * (numrho+2) + r + 1];
                }
            }
        }
    }

    // stage 2. find local maximums
    std::vector<int> _sort_buf;
    for(int r = 0; r < numrho; ++r)
        for(int n = 0; n < numangle; ++n)
        {
            int base = (n + 1) * (numrho + 2) + r + 1;
            if( accum[base] > threshold &&
                    accum[base] > accum[base - 1] && accum[base] >= accum[base + 1] &&
                    accum[base] > accum[base - numrho - 2] && accum[base] >= accum[base + numrho + 2] )
                _sort_buf.push_back(base);
        }

    // stage 3. sort the detected lines by accumulator value
    std::sort(_sort_buf.begin(), _sort_buf.end(), hough_cmp_gt(accum));

    // stage 4. store the first min(total,linesMax) lines to the output buffer
    linesMax = std::min(linesMax, static_cast<int>(_sort_buf.size()));
    double const scale = 1./(numrho + 2);

    //avoid the cost of reallocation and the cost of
    //checking the memory size when push_back
    lines.resize(linesMax);
    for(int i = 0; i < linesMax; ++i){
        int const idx = _sort_buf[i];
        //n is row number of the acc table, which corresponding to theta number
        int const n = cvFloor(idx * scale) - 1;
        int const r = idx - (n + 1) * (numrho + 2) - 1;
        LinePolar line;
        line.rho = (r - (numrho - 1) * 0.5f) * rho;
        line.angle = n * theta;
        lines[i] = cv::Vec2f(line.rho, line.angle);
    }
}

int main()
{
    cv::Mat image = cv::imread("/Users/Qt/program/blogsCodes/pic/balls00.jpg", CV_LOAD_IMAGE_GRAYSCALE);
    if(image.empty()){
        std::cerr<<"input is empty"<<std::endl;
        return -1;
    }

    cv::Mat contours;
    cv::Canny(image, contours, 125, 350);
    cv::Mat contoursInv;
    cv::threshold(contours,contoursInv, 128,255,cv::THRESH_BINARY_INV);

    {
        timeEstimate<> t;
        for(size_t i = 0; i != 50; ++i){
            std::vector<cv::Vec2f> lines;
            HoughLinesStandard_RF(contours, 1, 3.1415926/180, 80, lines, 35000);
        }
    }

    {
        timeEstimate<> t;
        for(size_t i = 0; i != 50; ++i){
            std::vector<cv::Vec2f> lines;
            HoughLinesStandard_CV(contours, 1, 3.1415926/180, 80, lines, 35000);
        }
    }

    return 0;
}

