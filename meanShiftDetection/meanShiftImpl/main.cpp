#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

template<typename Func>
float moment(cv::Mat const &input, Func func)
{
    float sum = 0;
    for(int row = 0; row != input.rows; ++row){
        auto ptr = input.ptr<float>(row);
        for(int col = 0; col != input.cols; ++col){
            sum += func(ptr[col], row, col);
        }
    }

    return sum;
}

int main()
{
    cv::Mat input = (cv::Mat_<float>(3,3) <<
                     1, 2, 3,
                     4, 5, 6,
                     7, 8, 9);

    cv::Moments m = cv::moments(input);
    std::cout<<m.m00<<", "<<m.m10<<", "<<m.m01<<std::endl;

    float const m00 = moment(input, [](float data, int, int)
    {
        return data;
    });

    float const m10 = moment(input, [](float data, int, int col)
    {
        return data * col;
    });

    float const m01 = moment(input, [](float data, int row, int)
    {
        return data * row;
    });

    std::cout<<m00<<", "<<m10<<", "<<m01<<std::endl;



    return 0;
}

