#include "bg_subtractor.hpp"
#include "bg_subtractor_config.hpp"
#include "config_parser.hpp"
#include "rcnn_instance_segment.hpp"
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
void count_object(cv::FileStorage const &fs, Item id, bg_subtractor *bsub, Detector &detector)
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
                             cv::VideoWriter::fourcc('M','J','P','G'),
                             24.0, cv::Size(frame.cols, frame.rows));
            }
            int people_count = 0;
            if(bsub){
                 auto results = bsub->apply(frame);
                 frame = results.foreground_;
            }
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
            auto const key = std::tolower(cv::waitKey(wait_between_frame_ms));
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

void segment_object(cv::FileStorage const &fs, int width_reize_to, rcnn_instance_segment &segmenter)
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
                             cv::VideoWriter::fourcc('M','J','P','G'),
                             24.0, cv::Size(frame.cols, frame.rows));
            }
            double const ratio = width_reize_to/static_cast<double>(frame.cols);
            auto const psize = cv::Size(static_cast<int>(frame.cols * ratio),
                                        static_cast<int>(frame.rows * ratio));
            cout<<"psize:"<<psize<<std::endl;
            cv::resize(frame, frame, psize);
            segmenter.detect(frame);
            if(vwriter.isOpened()){
                vwriter<<frame;
            }
            cv::imshow("frame", frame);
            auto const key = std::tolower(cv::waitKey(wait_between_frame_ms));
            if(key == 'q'){
                break;
            }
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
        auto dnode = fs["detector"];
        auto const model_params = dnode["model_params"].string();
        auto const model_prototxt = dnode["model_arch"].string();
        auto const confident = static_cast<float>(dnode["min_confident"].real());
        std::unique_ptr<bg_subtractor> bsub;
        if(fs["use_bg_subtractor"].real() > 0.0){
            bsub = std::make_unique<bg_subtractor>(parse_bg_subtractor_config(argv[1]));
        }
        if(fs["mode"].string() == "mobile_net_ssd"){
            ssd_detector detector(model_params, model_prototxt, confident);
            count_object(fs, voc_item_type::person, bsub.get(), detector);
        }else if(fs["mode"].string() == "yolov3_coco"){                        
            auto const non_max_threshold = static_cast<float>(dnode["non_maxima_threshold"].real());
            cv::Size const process_size(static_cast<int>(dnode["process_width"].real()),
                    static_cast<int>(dnode["process_height"].real()));
            yolov3_detector detector(model_params, model_prototxt, confident, process_size, non_max_threshold);        
            count_object(fs, coco_item_type::person, bsub.get(), detector);
        }else if(fs["mode"].string() == "rcnn"){
            auto const mask_threshold = static_cast<float>(dnode["mask_threshold"].real());
            auto const width_reize_to = static_cast<int>(dnode["width_reize_to"].real());
            rcnn_instance_segment ris(model_params, model_prototxt, confident, mask_threshold);
            segment_object(fs, width_reize_to, ris);
        }
    }else{
        cerr<<"cannot open file:"<<argv[1]<<endl;
    }


    return 0;
}catch(std::exception const &ex){
    cerr<<ex.what()<<endl;
}
