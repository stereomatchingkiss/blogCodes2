#include "visitor_identify.hpp"

#include <libs/mxnet/common.hpp>

#include <mxnet-cpp/MxNetCpp.h>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/video.hpp>

#include <iostream>
#include <string>

using namespace mxnet_aux;
using namespace std;

cv::VideoCapture create_video_capture(cv::FileStorage const &input)
{
    if(input["input_media"].isInt()){
        return cv::VideoCapture(static_cast<int>(input["input_media"].real()));
    }

    return cv::VideoCapture(input["input_media"].string());
}

int main(int argc, char *argv[])try
{
    if(argc < 2){
        cout<<"Must specify location of json. ex : mxnet_person_re_id settings.json"<<endl;
        return -1;
    }

    cv::FileStorage fs(argv[1], cv::FileStorage::READ);
    if(fs.isOpened()){
        auto capture = create_video_capture(fs);
        if(capture.isOpened()){
            auto const person_re_id_params = fs["person_re_id_params"].string();
            auto const person_re_id_symbol = fs["person_re_id_symbol"].string();
            auto const obj_det_params = fs["obj_det_params"].string();
            auto const obj_det_symbols = fs["obj_det_symbols"].string();
            visitor_identify viden(person_re_id_params, person_re_id_symbol, obj_det_params, obj_det_symbols);
            cv::Mat frame;
            while(1){
                capture>>frame;
                if(!frame.empty()){
                    auto const visitor_info = viden.detect_and_identify_visitors(frame);
                    for(auto const &vinfo : visitor_info){
                        cv::rectangle(frame, vinfo.roi_, {255, 0, 0});
                        auto const label = "id:" + to_string(vinfo.id_) + ", confident:" +
                                std::to_string(vinfo.confidence_);
                        cv::putText(frame, label,
                                    vinfo.roi_.tl(), cv::FONT_HERSHEY_SIMPLEX,
                                    1, {255,0,255}, 2);
                    }
                    cv::imshow("frame", frame);
                    auto const key = cv::waitKey(10);
                    if(key == 'q' || key == 'Q'){
                        break;
                    }
                }else{
                    break;
                }
            }
        }
    }

    return 0;
}catch(std::exception const &ex){
    cerr<<ex.what()<<endl;
}
