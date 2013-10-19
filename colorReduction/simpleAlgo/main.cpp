#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

void color_reduce(cv::Mat &input, cv::Mat &output, size_t div)
{
    if(input.data != output.data){
        output.create(input.size(), input.type());
    }

    uchar buffer[256];
    for(size_t i = 0; i != 256; ++i){
        buffer[i] = i / div * div + div / 2;
    }
    cv::Mat table(1, 256, CV_8U, buffer, sizeof(buffer));
    cv::LUT(input, table, output);
}

int main()
{
    cv::Mat src = cv::imread("/Users/Qt/program/blogsCodes/pic/perspective08.jpg");
    if (src.empty()){
        std::cerr<<"can't open image"<<std::endl;
        return - 1;
    }

    cv::Mat output;
    color_reduce(src, output, 64);

    cv::imshow("result", output);
    cv::imwrite("result.jpg", output);
    cv::imshow("src", src);
    cv::waitKey();

    return 0;
}

