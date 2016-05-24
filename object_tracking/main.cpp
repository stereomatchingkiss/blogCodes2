#include "object_tracking_test.hpp"

#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>

#include <iomanip>
#include <iostream>
#include <string>

void video_to_image(std::string const &video,
                    std::string const &save_to);

int main()
{    
    try{
        test_tiny_cnn();
        //test_cmt();
        //test_correlation_track();
        //test_fixed_size_trackers();
        //test_gmg();
        //test_hsv_trackers();
        //test_pedestrian_detection();
        //test_tracking_module();
    }catch(std::exception const &ex){
        std::cerr<<ex.what()<<"\n";
    }
}

void video_to_image(std::string const &video,
                    std::string const &save_to)
{    
    cv::VideoCapture cap;
    cap.open(video);
    if(!cap.isOpened()){
        std::cerr<<"cannot read video\n";
        return;
    }

    cv::Mat frame;
    for(size_t i = 0; ; ++i){
        cap>>frame;
        if(!frame.empty()){
            std::stringstream ss;
            ss<<save_to<<"/img"<<std::setfill('0')
             <<std::setw(5)<<i<<".jpg";
            cv::imwrite(ss.str(), frame);
        }else{
            break;
        }
    }
}
