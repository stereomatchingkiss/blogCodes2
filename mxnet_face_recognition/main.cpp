#include "face_detector_params.hpp"
#include "face_key_extractor_params.hpp"
#include "face_reg_info.hpp"
#include "face_recognition.hpp"

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>

#include <QDir>
#include <QFileInfo>

#include <mxnet-cpp/MxNetCpp.h>

#include <fstream>
#include <iostream>

using namespace cv;
using namespace std;


void put_text(cv::Mat &im,
              std::string const &label,
              cv::Point const &orig,
              cv::Scalar const &color = cv::Scalar(255, 0, 0))
{
    int constexpr fontface = cv::FONT_HERSHEY_DUPLEX;
    double constexpr scale = 0.5;
    int constexpr thickness = 1;
    int baseline = 0;
    double constexpr alpha = 0.6;

    cv::Size const text = cv::getTextSize(label, fontface, scale, thickness, &baseline);
    // make sure roi inside image region
    cv::Rect blend_rect = cv::Rect(orig + cv::Point(0, baseline),
                                   orig + cv::Point(text.width, -text.height)) & cv::Rect(0, 0, im.cols, im.rows);
    cv::Mat const roi = im(blend_rect);
    cv::Mat const blend(roi.size(), CV_8UC3, color);
    cv::addWeighted(blend, alpha, roi, 1.0 - alpha, 0.0, roi);
    cv::putText(im, label, orig, fontface, scale, cv::Scalar(255, 255, 255), thickness, 8);
}

void add_faces_into_db(std::string const &folder, face_recognition &fr)
{
    auto const imgs_path = QDir(folder.c_str()).entryInfoList();
    for(auto const &finfo : imgs_path){
        cout<<"add face in:"<<finfo.fileName().toStdString()<<endl;
        auto img = cv::imread(finfo.absoluteFilePath().toStdString());
        if(!img.empty()){
            if(fr.add_new_face(img, finfo.baseName().toStdString())){
                cout<<"add face:"<<finfo.fileName().toStdString()<<", success"<<endl;
            }
        }
    }
}

int main(int argc, char *argv[])try
{
    if(argc < 2){
        cout<<"Please specify the location of the json"<<endl;

        return -1;
    }

    using namespace dlib_tool;
    using namespace mxnet_tool;

    cv::FileStorage fs;
    fs.open(argv[1], cv::FileStorage::READ);
    if(fs.isOpened()){
        cv::VideoCapture cam;
        if(fs["video"].string().empty()){
            cam.open(0);
        }else{
            cam.open(fs["video"].string());
        }

        if(cam.isOpened()){
            face_detector_params face_det_params(fs["dlib_face_detect_model"], fs["dlib_shape_predictor_model"]);
            face_key_extractor_params face_key_ext_params(fs["deepinsight_model_params"],
                    fs["deepinsight_model_symbols"],
                    mxnet::cpp::Context::gpu(0));
            face_recognition fr(std::move(face_det_params), std::move(face_key_ext_params));
            add_faces_into_db(fs["face_key_folder"], fr);
            for(cv::Mat frame; ;cam>>frame){
                cam>>frame;
                if(!frame.empty()){
                    auto const results = fr.recognize_faces(frame);
                    for(auto const &fr_info : results){
                        rectangle(frame, fr_info.roi_, Scalar(0, 255, 0), 3);
                        put_text(frame, fr_info.id_ + ":" + std::to_string(fr_info.confident_),
                                 cv::Point(fr_info.roi_.x, fr_info.roi_.y));
                    }
                    cv::imshow("frame", frame);
                    int const key = std::tolower(cv::waitKey(30));
                    if(key == 'q'){
                        break;
                    }
                }else{
                    break;
                }
            }
        }else{
            cerr<<"Cannot open camera"<<endl;
        }
    }
    return 0;
}catch(std::exception const &ex){
    cerr<<"exception:"<<ex.what()<<"\n";
}
