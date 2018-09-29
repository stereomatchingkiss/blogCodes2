#include "common.hpp"
#include "object_detector.hpp"
#include "plot_object_detector_bboxes.hpp"

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>

#include <mxnet-cpp/ndarray.h>

#include <iostream>

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
            if(capture.open(fs["input_media"].string())){
                cv::Mat frame;
                while(1){
                    capture>>frame;
                    if(!frame.empty()){
                        obj_det.forward(frame);
                        plotter.plot(frame, obj_det.get_outputs(), true);

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
        }
    }else{
        cout<<"cannot open file:"<<argv[1]<<endl;
    }

    return 0;
}catch(std::exception const &ex){
    cerr<<ex.what()<<endl;
}
