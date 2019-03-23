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
    //create the functor to handle the exception when cv::VideoCapture fail
    //to capture the frame and wait 30 msec between each frame
    long long constexpr wait_msec = 30;
    ocv::camera::async_opencv_video_capture cl([&](std::exception const &ex)
    {
        //cerr of c++ is not a thread safe class, so we need to lock the mutex
        std::unique_lock<std::mutex> lock(emutex);
        std::cerr<<"camera exception:"<<ex.what()<<std::endl;
        return true;
    }, wait_msec);
    cl.open_url(argv[1]);

    //add listener to process captured frame
    cv::Mat img;
    cl.add_listener([&](cv::Mat input)
    {
        std::unique_lock<std::mutex> lock(emutex);        
        img = input;
    }, &emutex);

    //execute the task(s)
    cl.run();

    //We must display the captured image at main thread but not
    //in the listener, because every manipulation related to gui
    //must perform in the main thread(it also called gui thread)
    for(int finished = false; finished != 'q';){
        finished = std::tolower(cv::waitKey(30));
        std::unique_lock<std::mutex> lock(emutex);
        if(!img.empty()){
            cv::imshow("frame", img);
        }
    }    
}
