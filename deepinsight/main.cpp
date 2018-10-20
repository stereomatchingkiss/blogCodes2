#include "face_detector.hpp"

#include "face_key.hpp"
#include "face_key_extractor.hpp"

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>

#include <mxnet-cpp/MxNetCpp.h>

#include <fstream>
#include <iostream>

using namespace cv;
using namespace std;

cv::FileStorage fs;
bool aligned_face = false;

mxnet_tool::face_key get_face_key(cv::Mat const &input,
                                  dlib_tool::face_detector &face_det,
                                  mxnet_tool::face_key_extractor &key_extractor)
{
    if(aligned_face){
        auto const face_info_list = face_det.forward(input);
        if(!face_info_list.empty()){
            return key_extractor.forward(face_info_list[0].face_aligned_);
        }
    }

    return key_extractor.forward(input);
}

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

void compare_similarity_on_images(mxnet_tool::face_key const &key_face_key,
                                  dlib_tool::face_detector &face_det,
                                  mxnet_tool::face_key_extractor &key_extractor)
{
    for(size_t i = 0; i != 100; ++i){
        std::stringstream ss;
        auto const key_face = cv::imread("my_" +  std::to_string(i)  + ".jpg");
        if(key_face.empty()){
            cout<<i<<":cannot load face"<<endl;
            continue;
        }
        auto const key_face_key_2 = get_face_key(key_face, face_det, key_extractor);
        cout<<i<<":"<<key_face_key.similarity(key_face_key_2)<<endl;
    }
}

bool rectangle_inside_image(dlib::rectangle const &rect, cv::Size const &img_size)
{
    return rect.left() >= 0 && rect.top() >= 0 &&
            (static_cast<int>(rect.width() + rect.left()) < img_size.width) &&
            (static_cast<int>(rect.height() + rect.top()) < img_size.height);
}

void show_face_similarity(mxnet_tool::face_key const &key_face_key,
                          cv::Mat &frame,
                          dlib_tool::face_detector &face_det,
                          mxnet_tool::face_key_extractor &key_extractor)
{
    auto const face_info_list = face_det.forward(frame);
    for(auto const &finfo : face_info_list){
        auto const &rect = finfo.rect_.rect;
        if(rectangle_inside_image(rect, frame.size())){
            auto const &cv_rect = cv::Rect(rect.left(), rect.top(), rect.width(), rect.height());
            cv::rectangle(frame, cv_rect, cv::Scalar(0, 255, 0), 3);
            mxnet_tool::face_key face_key;
            if(aligned_face){
                face_key = key_extractor.forward(finfo.face_aligned_);
            }else{
                face_key = key_extractor.forward(frame(cv_rect));
            }
            put_text(frame,
                     std::to_string(key_face_key.similarity(face_key)),
                     cv::Point(rect.left(), rect.top()));
        }
    }
}

int main(int argc, char *argv[])try
{
    if(argc < 2){
        cout<<"Please specify the location of the json"<<endl;

        return -1;
    }

    fs.open(argv[1], cv::FileStorage::READ);
    if(fs.isOpened()){
        aligned_face = static_cast<bool>(fs["aligned_face"].real());
        cv::VideoCapture cam;
        if(fs["video"].string().empty()){
            cam.open(0);
        }else{
            cam.open(fs["video"].string());
        }

        cv::Mat const key_face = cv::imread(fs["key_face"].string());
        if(cam.isOpened() && !key_face.empty()){
            dlib_tool::face_detector face_det(fs["dlib_face_detect_model"].string(),
                    fs["dlib_shape_predictor_model"].string());
            mxnet_tool::face_key_extractor key_extractor(fs["deepinsight_model_params"],
                    fs["deepinsight_model_symbols"],
                    mxnet::cpp::Context::gpu(0));
            auto const key_face_key = get_face_key(key_face, face_det, key_extractor);
            //compare_similarity_on_images(key_face_key, face_det, key_extractor);
            //return 0;
            if(!key_face_key.is_empty_key()){
                for(cv::Mat frame; ;cam>>frame){
                    cam>>frame;
                    if(frame.empty()){
                        break;
                    }else{
                        show_face_similarity(key_face_key, frame, face_det, key_extractor);
                        cv::imshow("frame", frame);
                        int const key = cv::waitKey(30);
                        if(key == 'q' || key == 'Q'){
                            break;
                        }
                    }
                }
            }else{
                cout<<"face key of key face is empty"<<endl;
            }
        }else{
            cerr<<"Cannot open camera"<<endl;
        }
    }
    return 0;
}catch(std::exception const &ex){
    cerr<<"exception:"<<ex.what()<<"\n";
}
