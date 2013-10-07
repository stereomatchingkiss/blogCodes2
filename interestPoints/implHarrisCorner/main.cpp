#include <iostream>
#include <stdexcept>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "harrisCornerImpl.hpp"

int main()
{    
    try{
        cv::Mat color_input = cv::imread("/Users/Qt/program/blogsCodes/pic/church01.jpg");
        if(color_input.empty()){
            std::cerr<<"can't open image"<<std::endl;
            return -1;
        }

        cv::Mat corner_strength;
        cv::Mat gray_input;
        cv::cvtColor(color_input, gray_input, CV_BGR2GRAY);
        harris_corners(gray_input, corner_strength);
        cv::Mat thresh;
        cv::threshold(corner_strength, thresh, 0.0001, 255, cv::THRESH_BINARY);

        for(int row = 0; row != thresh.rows; ++row){
            auto thresh_ptr = thresh.ptr<float>(row);
            for(int col = 0; col != thresh.cols; ++col){
                if(thresh_ptr[col] != 0){
                    cv::circle(color_input, cv::Point(col, row), 3, cv::Scalar(255));
                }
            }
        }

        cv::imwrite("result.jpg", color_input);
        cv::waitKey();
    }catch(std::exception const &ex){
        std::cerr<<ex.what()<<std::endl;
    }

    return 0;
}

