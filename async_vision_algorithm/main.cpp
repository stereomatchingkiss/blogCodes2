#include "async_to_gray_algo.hpp"

#include <ocv_libs/camera/async_opencv_video_capture.hpp>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

#include <iostream>
#include <mutex>

int main(int argc, char *argv[])
{
    if(argc != 2){
        std::cerr<<"must enter url of media\n";
        return -1;
    }

    std::mutex gmutex;
    //create the functor to handle the exception when cv::VideoCapture fail
    //to capture the frame and wait 30 msec between each frame
    long long constexpr wait_msec = 30;
    ocv::camera::async_opencv_video_capture<> cl([&](std::exception const &ex)
    {
        //cerr of c++ is not a thread safe class, so we need to lock the mutex
        std::lock_guard<std::mutex> lock(gmutex);
        std::cerr<<"camera exception:"<<ex.what()<<std::endl;

        return true;
    }, wait_msec);
    cl.open_url(argv[1]);

    cv::Mat gray_img;
    //we need the gmutex to prevent race condition of gray_img
    //because async_to_gray_algo convert image to gray in another thread
    async_to_gray_algo  atgl(gray_img, gmutex);
    cl.add_listener([&](cv::Mat input)
    {
        atgl.process(input);
    }, &atgl);

    //execute the task(s)
    cl.run();

    //We must display the captured image at main thread but not
    //in the listener, because every manipulation related to gui
    //must perform in the main thread(it also called gui thread)
    for(int finished = false; finished != 'q';){
        finished = std::tolower(cv::waitKey(30));
        std::lock_guard<std::mutex> lock(gmutex);
        //std::cout<<"process gray image done"<<std::endl;
        if(!gray_img.empty()){
            cv::imshow("frame", gray_img);
        }
    }
}
