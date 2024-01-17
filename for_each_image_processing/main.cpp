#include <timeEstimate.hpp>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

#include <algorithm>
#include <execution>
#include <iostream>

using pixel_type = cv::Point3_<uint8_t>;

inline void simple_threshold(pixel_type &pixel, const int * position)
{
    //x,y,z equal to b,g,r of the cv::Mat pixel
    if(std::pow(static_cast<double>(pixel.x)/10.0 , 2.5) > 50){
        pixel.x = 255;
        pixel.y = 255;
        pixel.z = 255;
    }else{
        pixel.x = 125;
        pixel.y = 125;
        pixel.z = 125;
    }
}


struct simple_threshold_func
{
    void operator ()(pixel_type &pixel, const int * position) const noexcept
    {
        simple_threshold(pixel, position);
    }
};

void create_random_mat(cv::Mat &img)
{
    cv::randu(img, cv::Scalar(0, 0, 0), cv::Scalar(255, 255, 255));
}

int main()
{
    cv::Mat img(640, 480, CV_8UC3);
    create_random_mat(img);

    {
        timeEstimate es;
        img.forEach<pixel_type>(simple_threshold_func());
    }

    create_random_mat(img);
    //lambda expression work too
    {
        timeEstimate es;
        img.forEach<pixel_type>([](auto &pixel, int const *position)
                                {
                                    simple_threshold(pixel, position);
                                });
    }

    create_random_mat(img);
    {
        timeEstimate es;
        std::for_each(std::execution::par, img.ptr<cv::Vec3b>(), img.ptr<cv::Vec3b>() + img.total(), [](auto &pixel)
                      {
                          if(std::pow(static_cast<double>(pixel[0])/10.0 , 2.5) > 50){
                              pixel = 255;
                          }else{
                              pixel = 125;
                          }
                      });
    }

    return 0;
}
