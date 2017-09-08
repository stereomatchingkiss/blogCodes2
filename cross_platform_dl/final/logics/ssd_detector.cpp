#include "ssd_detector.hpp"

#include <QDebug>

#include <type_traits>

using namespace cv;
using namespace std;

ssd_detector::ssd_detector(const std::string &model_bin, const std::string &proto_txt,
                           std::vector<std::string> class_names, bool swap_rb):
    class_names_(std::move(class_names)),
    net_(cv::dnn::readNetFromCaffe(proto_txt, model_bin)),
    swap_rb_(swap_rb)
{
}

ssd_detector::ssd_detector(const string &model_bin, const string &proto_txt, bool swap_rb):
    class_names_{"background",
                 "aeroplane", "bicycle", "bird", "boat",
                 "bottle", "bus", "car", "cat", "chair",
                 "cow", "diningtable", "dog", "horse",
                 "motorbike", "person", "pottedplant",
                 "sheep", "sofa", "train", "tvmonitor"},
    net_(cv::dnn::readNetFromCaffe(proto_txt, model_bin)),
    swap_rb_(swap_rb)
{

}

ssd_detector::~ssd_detector()
{

}

Rect ssd_detector::center_crop_rect(Size const in_size) const
{
    Size crop_size;
    if(in_size.width > in_size.height){
        crop_size = Size(in_size.height, in_size.height);
    }else{
        crop_size = Size(in_size.width, in_size.width);
    }

    return Rect(Point((in_size.width - crop_size.width) / 2,
                      (in_size.height - crop_size.height) / 2),
                crop_size);
}

ssd_detector::result_type ssd_detector::detect(const cv::Mat &input, float min_confident)
{
    constexpr size_t in_width = 300;
    constexpr size_t in_height = 300;
    constexpr float in_scale_factor = 0.007843f;
    constexpr float mean_val = 127.5;

    //qDebug()<<"blob from image";
    Mat const input_blob = dnn::blobFromImage(input, in_scale_factor, cv::Size(in_width, in_height),
                                              mean_val, swap_rb_);
    //qDebug()<<"set input blob";
    net_.setInput(input_blob, "data");
    //qDebug()<<"detection out";
    Mat detection = net_.forward("detection_out");
    //qDebug()<<"create detection mat";
    Mat detection_mat(detection.size[2], detection.size[3], CV_32F, detection.ptr<float>());
    //qDebug()<<"center crop";
    Mat const crop_input = input(center_crop_rect(input.size()));
    //qDebug()<<"shift points";
    Point const shift = Point((input.cols - crop_input.cols) / 2,
                              (input.rows - crop_input.rows) / 2);

    result_type::first_type mapper;
    for(int i = 0; i < detection_mat.rows; ++i){
        //qDebug()<<"detection mat row:"<<i;
        float const confidence = detection_mat.at<float>(i, 2);
        if(confidence > min_confident){            
            size_t const object_class = static_cast<size_t>(detection_mat.at<float>(i, 1));
            int const xleft_bottom = static_cast<int>(detection_mat.at<float>(i, 3) * crop_input.cols);
            int const yleft_bottom = static_cast<int>(detection_mat.at<float>(i, 4) * crop_input.rows);
            int const xright_top = static_cast<int>(detection_mat.at<float>(i, 5) * crop_input.cols);
            int const yright_top = static_cast<int>(detection_mat.at<float>(i, 6) * crop_input.rows);

            Rect const bounding_box(xleft_bottom + shift.x, yleft_bottom + shift.y,
                                    xright_top - xleft_bottom, yright_top - yleft_bottom);
            auto it = mapper.find(class_names_[object_class]);
            if(it != std::end(mapper)){
                it->second.emplace_back(bounding_box);
            }else{
                std::vector<cv::Rect> values;
                values.emplace_back(bounding_box);
                mapper.emplace(std::make_pair(class_names_[object_class], std::move(values)));
            }
        }
    }

    //qDebug()<<"all done";

    return std::make_pair(std::move(mapper), shift);
}

void ssd_detector::set_class_names(std::vector<string> class_names)
{
    class_names_ = std::move(class_names);
}
