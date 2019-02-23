#include "../libs/face/age_gender/insight_age_gender_info.hpp"
#include "../libs/face/age_gender/insight_age_gender_predict.hpp"
#include "../libs/face/age_gender/insight_age_gender_predict_params.hpp"

#include "../libs/face/detect/dlib_cnn_face_detector.hpp"
#include "../libs/face/detect/dlib_cnn_face_detector_params.hpp"

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>

#include <mxnet-cpp/MxNetCpp.h>

#include <QDir>

#include <iostream>

using namespace cv;
using namespace std;

using namespace ocv::face;

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

void put_text(insight_age_gender_info const &info, cv::Rect const &roi, Mat &inout)
{
    auto const gender = info.gender_ == gender_info::male_ ? "male" : "female";
    auto const msg = "age:" + std::to_string(info.age_) + ",gender:" + gender;
    put_text(inout, msg, cv::Point(roi.x, roi.y));
    put_text(inout, msg, cv::Point(roi.x, roi.y + roi.height));
}

void record_video(cv::Mat const &frame, std::string const &save_output_as, int fps, VideoWriter &vwriter)
{
    if(!save_output_as.empty()){
        if(!vwriter.isOpened()){
            vwriter.open(save_output_as,
                         cv::VideoWriter::fourcc('H','2','6','4'),
                         fps, cv::Size(frame.cols, frame.rows));
        }
        if(vwriter.isOpened()){
            vwriter<<frame;
        }
    }
}

dlib_cnn_face_detector create_face_detector(FileStorage const &fs)
{
    dlib_cnn_face_detector_params face_det_params(fs["dlib_face_detect_model"], fs["dlib_shape_predictor_model"]);
    face_det_params.face_detect_width_ = 800;
    return dlib_cnn_face_detector(face_det_params);
}

insight_age_gender_predict create_age_gender_predict(FileStorage const &fs)
{
    insight_age_gender_predict_params params(fs["deepinsight_model_params"], fs["deepinsight_model_symbols"],
            mxnet::cpp::Context::gpu(0));
    return insight_age_gender_predict(params.model_params_, params.model_symbols_, params.context_, params.shape_);
}

Rect mmod_to_rect(cv::Mat const &frame, dlib::mmod_rect const &mmod)
{
    auto const &drect = mmod.rect;
    Rect rect;
    rect.x = std::max(0, static_cast<int>(drect.left()));
    rect.y = std::max(0, static_cast<int>(drect.top()));
    rect.height = std::min(frame.rows, static_cast<int>(drect.height()));
    rect.width = std::min(frame.cols, static_cast<int>(drect.width()));

    return rect;
}

void process_image(dlib_cnn_face_detector &fdet, insight_age_gender_predict &age_gender_predict, cv::Mat &inout)
{
    auto const face_det_info = fdet.forward(inout);
    auto const ag_info = age_gender_predict.forward(face_det_info.face_aligned_);
    for(size_t i = 0; i != ag_info.size(); ++i){
        Rect const rect = mmod_to_rect(inout, face_det_info.rect_[i]);
        rectangle(inout, rect, Scalar(0, 255, 0), 3);
        put_text(ag_info[i], rect, inout);
    }
}

void process_images_in_folder(FileStorage const &fs)
{
    auto const image_folder = fs["image_folder"].string();
    if(!image_folder.empty()){
        auto fdet = create_face_detector(fs);
        auto age_gender_predict = create_age_gender_predict(fs);
        auto dir = QDir(image_folder.c_str());
        auto const imgs_path = QDir(image_folder.c_str()).entryInfoList(QDir::NoDotAndDotDot | QDir::Files);
        std::string const save_images_at(fs["save_images_at"].string());
        QDir().mkpath(save_images_at.c_str());
        for(auto const &finfo : imgs_path){
            cout<<"process image:"<<finfo.fileName().toStdString()<<endl;
            auto img = cv::imread(finfo.absoluteFilePath().toStdString());
            if(!img.empty()){
                process_image(fdet, age_gender_predict, img);
                imwrite(save_images_at + "/" + finfo.completeBaseName().toStdString() + ".png", img);
            }
        }
    }
}

void process_video(FileStorage const &fs)
{
    cv::VideoCapture cam;
    if(fs["video"].string().empty()){
        cam.open(0);
    }else{
        cam.open(fs["video"].string());
    }

    if(cam.isOpened()){
        auto fdet = create_face_detector(fs);
        auto age_gender_predict = create_age_gender_predict(fs);
        auto const save_video_as = fs["save_video_as"].string();
        auto const fps = static_cast<int>(fs["save_video_fps"].real());
        cv::VideoWriter vwriter;

        for(cv::Mat frame; ;){
            cam>>frame;
            if(!frame.empty()){
                process_image(fdet, age_gender_predict, frame);
                record_video(frame, save_video_as, fps, vwriter);

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

int main(int argc, char *argv[])try
{
    if(argc < 2){
        cerr<<"Please specify the location of the json"<<endl;
        return -1;
    }

    cv::FileStorage fs;
    fs.open(argv[1], cv::FileStorage::READ);
    if(fs.isOpened()){
       process_images_in_folder(fs);
       process_video(fs);
    }
    return 0;
}catch(std::exception const &ex){
    cerr<<"exception:"<<ex.what()<<"\n";
}
