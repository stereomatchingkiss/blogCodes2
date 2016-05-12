#include "object_tracking_test.hpp"
#include "correlation_trackers.hpp"
#include "CppMT/CMT.h"


#include <ocv_libs/saliency/utility.hpp>

#include <dlib/dir_nav.h>

#include <opencv2/core/utility.hpp>
#include <opencv2/bgsegm.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/video/background_segm.hpp>
#include <opencv2/tracking.hpp>

#include <iostream>
#include <sstream>
#include <vector>

namespace{

void correlation_tracker_test(std::string const &folder,
                              size_t frame_num,
                              std::vector<cv::Rect> const &roi)
{
    auto files =
            dlib::get_files_in_directory_tree(folder,
                                              dlib::match_ending(".jpg"));
    std::sort(std::begin(files), std::end(files));

    auto img = cv::imread(folder + "/" +
                          files[frame_num].name());
    correlation_tracker tracker;
    for(size_t i = 0; i != roi.size(); ++i){
        tracker.add(img, roi[i]);
    }

    std::vector<cv::Scalar> const colors{{255, 0, 0},
                                         {0, 255, 0},
                                         {0, 0, 255},
                                         {255, 255, 0}};

    for(size_t i = frame_num + 1; i != files.size(); ++i){
        img = cv::imread(folder + "/" + files[i].name());
        if(!img.empty()){
            tracker.update(img);
            auto const positions = tracker.get_position();
            for(size_t j = 0; j != positions.size(); ++j){
                cv::rectangle(img, positions[j], colors[j]);
            }
            cv::imshow("tracking demo", img);
            int const key = cv::waitKey(10);
            if(key == 'q'){
                break;
            }
        }else{
            break;
        }
    }
}

bool should_track_again(std::vector<cv::Rect> const &reference,
                        correlation_tracker const &tracker)
{
    if(tracker.empty()){
        return true;
    }
    if(reference.empty()){
        return false;
    }

    auto const track_region = tracker.get_position();
    for(auto const &trect : track_region){
        auto const should_track =
                std::none_of(std::begin(reference), std::end(reference),
                             [&](cv::Rect const &rec)
        {
                return ocv::saliency::calculate_iou(rec, trect) > 0.5;
    });
        if(should_track){
            return true;
        }
    }

    return false;
}

}

void test_correlation_track()
{
    //<image file='v1_frames\img00433.jpg'>
    //    <box top='139' left='145' width='40' height='92'/>
    //    <box top='141' left='287' width='46' height='86'/>
    //</image>
    correlation_tracker_test("v1_frames", {433},
    {{145,139,40,92}, {287,141,46,86}});

    //<image file='v2_frames\img00398.jpg'>
    //    <box top='156' left='201' width='46' height='103'/>
    //    <box top='170' left='281' width='32' height='69'/>
    //</image>
    //correlation_tracker_test("v2_frames", {398},
    //{{201,156,46,103}, {281,170,32,69}});

    //<image file='v3_frames\img01713.jpg'>
    //    <box top='162' left='100' width='40' height='105'/>
    //    <box top='176' left='164' width='41' height='88'/>
    //    <box top='188' left='259' width='30' height='63'/>
    //    <box top='166' left='318' width='40' height='101'/>
    //</image>
    //correlation_tracker_test("v3_frames", {1713},
    //{{100,162,40,105}, {164,176,41,88},
    // {259,188,30,63}, {318,166,40,101}});
}

void test_gmg()
{
    using namespace cv::bgsegm;

    cv::Ptr<cv::BackgroundSubtractor> fgbg =
            createBackgroundSubtractorGMG(20, 0.7);
    if(!fgbg){
        return;
    }

    cv::VideoCapture cap;
    cap.open("v2.mp4");
    if(!cap.isOpened()){
        std::cout<<"cannot read video"<<std::endl;
        return;
    }

    cv::Mat frame, fgmask;
    std::vector<std::vector<cv::Point>> contours;
    //auto const kernel =
    //        cv::getStructuringElement(cv::MORPH_RECT, {5,5});
    correlation_tracker tracker;
    std::vector<cv::Rect> rects;
    for(size_t i = 0; ;++i){
        cap >> frame;
        if(frame.empty()){
            break;
        }
        cv::blur(frame, frame, {3,3});
        fgbg->apply(frame, fgmask);
        //cv::morphologyEx(fgmask, fgmask,
        //                 cv::MORPH_OPEN, kernel);
        //cv::morphologyEx(fgmask, fgmask,
        //                 cv::MORPH_CLOSE, kernel);
        bool const segment_bg = i >= 300 ? (i % 15) == 0 : (i % 300) == 0;
        if(segment_bg){
            cv::findContours(fgmask, contours, cv::RETR_EXTERNAL,
                             cv::CHAIN_APPROX_SIMPLE);
            for(auto const &ct : contours){
                //cv::drawContours(fgmask, contours, i, {255, 0, 0});
                auto const Rect = cv::boundingRect(ct);
                if(Rect.area() > 1000){
                    rects.push_back(Rect);
                }
            }
        }
        if(segment_bg && should_track_again(rects, tracker)){
            tracker.clear();
            for(auto const &rect : rects){
                tracker.add(frame, rect);
                cv::rectangle(frame, rect, {255,0,0});
            }
        }else{
            tracker.update(frame);
            auto const positions = tracker.get_position();
            for(auto const &pos : positions){
                cv::rectangle(frame, pos, {255,0,0});
            }
        }

        if(segment_bg){
            rects.clear();
        }
        cv::imshow("frame", frame);
        cv::imshow("fg seg", fgmask);
        int c = cv::waitKey(30);
        if (c == 'q' || c == 'Q' || (c & 255) == 27){
            break;
        }
    }
}

void test_tracking_module()
{
    cv::MultiTrackerTLD trackers;
    std::vector<cv::Rect2d> const objects
    {{145,139,40,92}, {287,141,46,86}};

    size_t const start = 500;
    std::string const video = "v1_frames";

    auto files =
            dlib::get_files_in_directory_tree(video,
                                              dlib::match_ending(".jpg"));
    std::sort(std::begin(files), std::end(files));

    auto frame = cv::imread(video + "/" +
                            files[start].name());
    auto const rect = cv::selectROI("pp", frame,
                                    false, false);
    trackers.addTarget(frame, rect, "KCF");
    for(auto const &obj : objects){
        //trackers.addTarget(frame, obj, "KCF");
    }    

    for(size_t i = start+1; i != files.size(); ++i){
        frame = cv::imread(video + "/" + files[i].name());
        if(!frame.empty()){     
            trackers.update(frame);
            for(size_t j = 0; j < trackers.targetNum; ++j){
                cv::rectangle(frame, trackers.boundingBoxes[j],
                              trackers.colors[j]);
            }
            cv::imshow("tracker", frame);
            int const key = cv::waitKey(30);
            if(key == 'q'){
                break;
            }
        }else{
            break;
        }
    }
}

void test_cmt()
{
    //<image file='v1_frames\img00433.jpg'>
    //    <box top='139' left='145' width='40' height='92'/>
    //    <box top='141' left='287' width='46' height='86'/>
    //</image>

    //<image file='v2_frames\img00398.jpg'>
    //    <box top='156' left='201' width='46' height='103'/>
    //    <box top='170' left='281' width='32' height='69'/>
    //</image>
    //{{201,156,46,103}, {281,170,32,69}});

    //<image file='v3_frames\img01713.jpg'>
    //    <box top='162' left='100' width='40' height='105'/>
    //    <box top='176' left='164' width='41' height='88'/>
    //    <box top='188' left='259' width='30' height='63'/>
    //    <box top='166' left='318' width='40' height='101'/>
    //</image>

    std::string const video = "v1_frames";
    size_t const start = 433;

    cmt::CMT cmt;
    std::vector<cv::Rect2d> objects
    {cv::Rect2d{100,162,40,105}};

    auto files =
            dlib::get_files_in_directory_tree(video,
                                              dlib::match_ending(".jpg"));
    std::cout<<"num of files "<<files.size()<<std::endl;
    std::sort(std::begin(files), std::end(files));

    auto frame = cv::imread(video + "/" +
                            files[start].name());

    cv::Mat gframe;
    cv::cvtColor(frame, gframe, CV_BGR2GRAY);
    //cmt.initialize(gframe, objects[0]);
    auto const box = cv::selectROI("ff", frame, false, false);
    std::cout<<box<<std::endl;
    cmt.initialize(gframe, box);

    for(size_t i = start+1; i != files.size(); ++i){
        frame = cv::imread(video + "/" + files[i].name());
        if(!frame.empty()){
            cv::cvtColor(frame, gframe, CV_BGR2GRAY);
            cmt.processFrame(gframe);
            auto const &pts = cmt.points_active;
            for(auto const pt : pts){
                cv::circle(frame, pt, 1, {255,0,0});
            }

            auto const rect = cmt.bb_rot.boundingRect();
            cv::rectangle(frame, rect, {255, 0, 0});
            cv::imshow("tracker", frame);
            int const key = cv::waitKey(30);
            if(key == 'q'){
                break;
            }
        }else{
            break;
        }
    }//*/
}
