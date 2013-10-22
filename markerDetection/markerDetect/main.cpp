#include <iostream>

#include <opencv2/highgui/highgui.hpp>

#include "markerDetector.hpp"

int main()
{
    cv::Mat color_input = cv::imread("/Users/Qt/program/blogsCodes/pic/marker00.png");
    if(color_input.empty()){
        std::cerr<<"can't open image"<<std::endl;
        return -1;
    }

    markerDetector detect;
    cv::Mat output;
    detect.process_frame(color_input, output);

    cv::imshow("", output);
    cv::waitKey();

    return 0;
}

