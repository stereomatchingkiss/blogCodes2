#include "common.hpp"
#include "object_detector.hpp"
#include "plot_object_detector_bboxes.hpp"

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

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

int main()try
{    
    std::string const prefix("C:/Users/yyyy/.mxnet/models/yolo3_darknet53_coco");
    object_detector obj_det(prefix + ".params", prefix + ".json", mxnet::cpp::Context::gpu(0));

    cv::Mat image, resize_img;
    std::string const path("C:/Users/yyyy/programs/Qt/computer_vision_dataset/phone_images/car/");
    std::tie(image, resize_img) = load_image(path + "car_1.jpg");
    obj_det.forward(resize_img);

    auto const coco_class_names = create_coco_obj_detection_labels();

    viz::plot_object_detector_bboxes plotter(coco_class_names);
    plotter.set_normalize_size(cv::Size(320, 256));
    plotter.plot(resize_img, obj_det.get_outputs());

    cv::imshow("plot", resize_img);
    cv::waitKey();

    return 0;
}catch(std::exception const &ex){
    cerr<<ex.what()<<endl;
}
