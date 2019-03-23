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

    std::mutex emutex;
    ocv::camera::async_opencv_video_capture cl([&](std::exception const &ex)
    {
        std::unique_lock<std::mutex> lock(emutex);
        std::cerr<<"camera exception:"<<ex.what()<<std::endl;
        return true;
    });
    cl.open_url(argv[1]);

    cv::Mat img;    
    cl.add_listener([&](cv::Mat input)
    {
        std::unique_lock<std::mutex> lock(emutex);        
        img = input;
    }, &emutex);
    cl.run();

    for(int finished = false; finished != 'q';){
        finished = std::tolower(cv::waitKey(30));
        std::unique_lock<std::mutex> lock(emutex);
        if(!img.empty()){
            cv::imshow("frame", img);
        }
    }    
}
