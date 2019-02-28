#include "../libs/face/detect/dlib_cnn_face_detector.hpp"
#include "../libs/face/detect/dlib_cnn_face_detector_params.hpp"

#include "../libs/face/pose/head_pose_estimation.hpp"
#include "../libs/face/pose/head_pose_info.hpp"

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

using namespace ocv::face;

dlib_cnn_face_detector create_face_detector(FileStorage const &fs)
{
    dlib_cnn_face_detector_params face_det_params(fs["dlib_face_detect_model"],
            fs["dlib_shape_predictor_model"],
            fs["face_detect_confidence"].real());
    face_det_params.face_detect_width_ = 600;
    return dlib_cnn_face_detector(face_det_params);
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

int main(int argc, char **argv)
{
    if(argc < 2){
        cout<<"Please specify the location of the json"<<endl;

        return -1;
    }

    VideoCapture vcap;
    if(vcap.open(0)){
        FileStorage fs;
        if(fs.open(argv[1], cv::FileStorage::READ)){
            auto fdet = create_face_detector(fs);
            ocv::face::head_pose_estimation hpe;
            for(Mat frame; ;){
                vcap>>frame;
                if(!frame.empty()){
                    auto const fdet_result = fdet.forward(frame);
                    for(size_t i = 0; i != fdet_result.face_aligned_.size(); ++i){
                        Rect const rect = mmod_to_rect(frame, fdet_result.rect_[i]);
                        rectangle(frame, rect, Scalar(0, 255, 0), 3);

                        auto const hpose = hpe.estimate(fdet_result.shapes_[i], cv::Size(frame.cols, frame.rows));
                        put_text(frame, "roll:" + std::to_string(hpose.roll_) +
                                 ", yaw:" + std::to_string(hpose.yaw_) +
                                 ", pitch:" + std::to_string(hpose.pitch_),
                                 cv::Point(0, 30));
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
            cerr<<"cannot open json file:"<<argv[1]<<endl;
        }
    }else{
        cerr<<"cannot open camera"<<endl;
    }
}
