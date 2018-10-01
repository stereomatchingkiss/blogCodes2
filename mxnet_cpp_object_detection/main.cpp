#include "common.hpp"
#include "object_detector.hpp"
#include "plot_object_detector_bboxes.hpp"

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>

#include <mxnet-cpp/ndarray.h>

#include <iostream>
#include <chrono>

using namespace std;

std::pair<cv::Mat, cv::Mat> load_image(std::string const &location, cv::Size const &resize_to = cv::Size(320, 256))
{
    cv::Mat image = cv::imread(location, 1);
    if(image.empty()){
        cout<<"cannot open image:"<<endl;
        throw std::runtime_error("Cannot load image");
    }

    cv::Mat resize_img;
    cv::resize(image, resize_img, resize_to);

    return {image, resize_img};
}

int main(int argc, char *argv[])try
{    
    if(argc < 2){
        cout<<"Please specify the location of the json"<<endl;

        return -1;
    }

    cv::FileStorage fs(argv[1], cv::FileStorage::READ);
    if(fs.isOpened()){
        object_detector obj_det(fs["model_params"].string(), fs["model_symbols"].string(), mxnet::cpp::Context::gpu(0));
        viz::plot_object_detector_bboxes plotter(create_coco_obj_detection_labels(),
                                                 static_cast<float>(fs["detect_confidence"].real()));
        plotter.set_normalize_size(cv::Size(320, 256));
        if(fs["media_is_image"].real() == 1.0){
            cv::Mat image, resize_img;
            std::tie(image, resize_img) = load_image(fs["input_media"].string());
            obj_det.forward(resize_img);

            plotter.plot(resize_img, obj_det.get_outputs());

            cv::imshow("plot", resize_img);
            cv::waitKey();
        }else{
            cv::VideoCapture capture;
            cv::VideoWriter vwriter;
            //you need the dll "openh264-1.7.0-win64.dll"(for opencv3.4.2) in order to encode the
            //video by h264 encoder
            bool const can_open_video = vwriter.open("yolov3.mp4",
                                                     cv::VideoWriter::fourcc('H','2','6','4'),
                                                     24.0, cv::Size(640, 360));

            int frame_count = 0;
            double elapsed = 0.0;
            if(capture.open(fs["input_media"].string())){
                cv::Mat frame;
                while(1){
                    capture>>frame;
                    if(!frame.empty()){
                        ++frame_count;
                        auto const start = std::chrono::system_clock::now();
                        obj_det.forward(frame);
                        //must call wait because forward api of mxnet is async, else
                        //measurement of the speed of inference wouldn't be accurate
                        mxnet::cpp::NDArray::WaitAll();
                        auto const end = std::chrono::system_clock::now();
                        std::chrono::duration<double> const elapsed_seconds = end-start;
                        elapsed += elapsed_seconds.count();
                        plotter.plot(frame, obj_det.get_outputs(), true);
                        if(can_open_video){
                            vwriter<<frame;
                        }
                        cv::imshow("plot", frame);
                        int const key = cv::waitKey(10);
                        if(key == 'q'){
                            break;
                        }
                    }else{
                        break;
                    }
                }
            }
            cout<<"total elapsed:"<<elapsed<<", total frame:"<<frame_count<<endl;
            if(elapsed != 0.0){
                cout<<"fps:"<<frame_count/elapsed<<endl;
            }
        }
    }else{
        cout<<"cannot open file:"<<argv[1]<<endl;
    }

    return 0;
}catch(std::exception const &ex){
    cerr<<ex.what()<<endl;
}
