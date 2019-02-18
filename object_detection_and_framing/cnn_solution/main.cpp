#include "ssd_detector.hpp"
#include "yolov3_detector.hpp"

#include "../fps_estimator.hpp"

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#include <iostream>

using namespace cv;
using namespace std;

cv::VideoCapture create_video_capture(cv::FileStorage const &input)
{
    if(input["input_media"].isInt()){
        return cv::VideoCapture(static_cast<int>(input["input_media"].real()));
    }

    return cv::VideoCapture(input["input_media"].string());
}

template<typename Detector, typename Item>
void count_object(cv::FileStorage const &fs, Item id, Detector &detector)
{
    auto capture = create_video_capture(fs);
    int const wait_between_frame_ms = static_cast<int>(fs["wait_between_frame_ms"].real());
    Mat frame;
    fps_estimator fps_est_;
    cv::VideoWriter vwriter;
    while(1){
        fps_est_.start();
        capture>>frame;
        if(!frame.empty()){
            if(!vwriter.isOpened()){
                vwriter.open(fs["save_output_as"].string(),
                             cv::VideoWriter::fourcc('H','2','6','4'),
                             24.0, cv::Size(frame.cols, frame.rows));
            }
            int people_count = 0;
            auto const results = detector.detect(frame);
            for(auto const &vpair : results){
                if(vpair.first == id){
                    for(auto const &brect : vpair.second.boxes_){
                        cv::rectangle(frame, brect, {255, 0, 0}, 2);
                    }
                    people_count = static_cast<int>(vpair.second.boxes_.size());
                }
            }
            cv::putText(frame, std::to_string(people_count) + " people",
                        Point(0, 65), FONT_HERSHEY_SIMPLEX, 1, {255,0,255}, 2);
            cv::putText(frame, "fps: " + std::to_string(fps_est_.get_fps()),
                        Point(0, 30), FONT_HERSHEY_SIMPLEX, 1, {255,0,255}, 2);
            if(vwriter.isOpened()){
                vwriter<<frame;
            }
            cv::imshow("frame", frame);
            auto const key = cv::waitKey(wait_between_frame_ms);
            if(key == 'q'){
                break;
            }
            fps_est_.end();
        }else{
            cout<<"video end, break"<<endl;
            break;
        }
    }
}

int main(int argc, char *argv[])try
{
    if(argc != 2){
        cerr<<"Must enter the location of json\n";
        return -1;
    }

    cv::FileStorage fs(argv[1], cv::FileStorage::READ);
    if(fs.isOpened()){
        auto const model_params = fs["model_params"].string();
        auto const model_prototxt = fs["model_arch"].string();
        auto const confident = static_cast<float>(fs["min_confident"].real());
        if(fs["mode"].string() == "mobile_net_ssd"){
            ssd_detector detector(model_params, model_prototxt, confident);
            count_object(fs, voc_item_type::person, detector);
        }else if(fs["mode"].string() == "yolov3_coco"){
            auto const non_max_threshold = static_cast<float>(fs["non_maxima_threshold"].real());
            cv::Size const process_size(static_cast<int>(fs["process_width"].real()),
                    static_cast<int>(fs["process_height"].real()));
            yolov3_detector detector(model_params, model_prototxt, confident, process_size, non_max_threshold);
            count_object(fs, coco_item_type::person, detector);
        }
    }else{
        cerr<<"cannot open file:"<<argv[1]<<endl;
    }


    return 0;
}catch(std::exception const &ex){
    cerr<<ex.what()<<endl;
}
