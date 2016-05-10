#include "correlation_trackers.hpp"

#include <dlib/dir_nav.h>

#include <opencv2/bgsegm.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/video/background_segm.hpp>

#include <iostream>
#include <vector>

void correlation_tracker_test();
void gmg_test();

int main()
{          
}

void correlation_tracker_test()
{
    auto files =
            dlib::get_files_in_directory_tree("video_frames",
                                              dlib::match_ending(".jpg"));
    std::sort(std::begin(files), std::end(files));

    correlation_tracker tracker;
    auto img = cv::imread("video_frames/" + files[0].name());
    tracker.add_track(img, {93-38/2,110-86/2,38,86});
    for(size_t i = 1; i != files.size(); ++i){
        img = cv::imread("video_frames/" + files[i].name());
        if(!img.empty()){
            tracker.update(img);
            auto const position = tracker.get_position(0);
            cv::rectangle(img, position, {255, 0, 0});
            cv::imshow("img", img);
            cv::waitKey(30);
        }else{
            break;
        }
    }
}

void gmg_test()
{
    using namespace cv::bgsegm;

    cv::setNumThreads(-1);

    cv::Ptr<cv::BackgroundSubtractor> fgbg =
            createBackgroundSubtractorGMG(20, 0.7);
    cv::Ptr<cv::BackgroundSubtractor> mog2 =
            cv::createBackgroundSubtractorMOG2(500,16,false);
    if(!fgbg || !mog2){
        return;
    }

    cv::VideoCapture cap;
    cap.open("v1.mp4");
    if(!cap.isOpened()){
        std::cout<<"cannot read video"<<std::endl;
        return;
    }

    cv::Mat frame, fgmask, fgmask2, segm;
    while(1){
        cap >> frame;
        if(frame.empty()){
            break;
        }
        //fgbg->apply(frame, fgmask);
        mog2->apply(frame, fgmask2);
        frame.convertTo(segm, CV_8U, 0.5);
        //segm = frame + {100,100,0}
        //cv::add(frame, cv::Scalar{100,100,0}, segm, fgmask);
        cv::imshow("frame", frame);
        //cv::imshow("fg seg", segm);
        cv::imshow("mog2", fgmask2);
        int c = cv::waitKey(30);
        if (c == 'q' || c == 'Q' || (c & 255) == 27){
            break;
        }
    }
}
