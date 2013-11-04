#include <iostream>

#include <opencv2/highgui/highgui.hpp>

#include <utility/utility.hpp>

#include "detectObject.hpp"
#include "preprocessFace.hpp"

int main()
{
    cv::VideoCapture cam;
    cam.open(0);
    if(!cam.isOpened()){
        std::cerr<<"can't open camera"<<std::endl;
        return -1;
    }

    std::string const &prefix = "/Users/Qt/program/3rdLibs/openCV/opencv-2.4.6.1/data/";
    cv::Mat frame;
    cv::Mat copy_frame;
    detectObject detect_obj;
    //preprocessFace preprocess(prefix + "lbpcascades/lbpcascade_frontalface.xml",
    //                          prefix + "haarcascades/haarcascade_eye.xml",
    //                          prefix + "haarcascades/haarcascade_eye_tree_eyeglasses.xml");
    cv::CascadeClassifier cs(prefix + "lbpcascades/lbpcascade_frontalface.xml");
    while(1){
        if(!cam.read(frame)){
            std::cerr<<"can't read next frame"<<std::endl;
        }

        frame.copyTo(copy_frame);
        cv::Rect const rect = detect_obj.detect_largest_object(frame, cs, 320);
        OCV::draw_rect(frame, rect);

        cv::imshow("", frame);
        char const key = cv::waitKey(30);
        if(key == 'q'){
            break;
        }
    }


    return 0;
}
