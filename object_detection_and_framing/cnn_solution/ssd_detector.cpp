#include "ssd_detector.hpp"

using namespace cv;
using namespace std;

ssd_detector::ssd_detector(const string &model_bin,
                           const string &proto_txt,
                           float confidence,
                           bool swap_rb):
    class_names_{"background",
                 "aeroplane", "bicycle", "bird", "boat",
                 "bottle", "bus", "car", "cat", "chair",
                 "cow", "diningtable", "dog", "horse",
                 "motorbike", "person", "pottedplant",
                 "sheep", "sofa", "train", "tvmonitor"},
    confidence_(static_cast<float>(confidence)),
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

ssd_detector::result_type ssd_detector::detect(const cv::Mat &input)
{
    constexpr size_t in_width = 300;
    constexpr size_t in_height = 300;
    constexpr double in_scale_factor = 0.007843;
    constexpr double mean_val = 127.5;

    Mat const input_blob = dnn::blobFromImage(input, in_scale_factor, cv::Size(in_width, in_height),
                                              mean_val, swap_rb_);
    net_.setInput(input_blob, "data");    
    Mat detection = net_.forward("detection_out");    
    Mat detection_mat(detection.size[2], detection.size[3], CV_32F, detection.ptr<float>());    
    Mat const crop_input = input(center_crop_rect(input.size()));    
    result_type mapper;
    for(int i = 0; i < detection_mat.rows; ++i){        
        float const confidence = detection_mat.at<float>(i, 2);
        if(confidence > confidence_){
            size_t const object_class = static_cast<size_t>(detection_mat.at<float>(i, 1));
            int const xleft_bottom = static_cast<int>(detection_mat.at<float>(i, 3) * input.cols);
            int const yleft_bottom = static_cast<int>(detection_mat.at<float>(i, 4) * input.rows);
            int const xright_top = static_cast<int>(detection_mat.at<float>(i, 5) * input.cols);
            int const yright_top = static_cast<int>(detection_mat.at<float>(i, 6) * input.rows);

            Rect const bounding_box(xleft_bottom, yleft_bottom,
                                    xright_top - xleft_bottom, yright_top - yleft_bottom);
            auto const id = static_cast<voc_item_type>(object_class);
            auto it = mapper.find(id);
            if(it != std::end(mapper)){
                it->second.boxes_.emplace_back(bounding_box);
                it->second.confidences_.emplace_back(confidence);
            }else{
                std::vector<cv::Rect> values;
                predict_resutls presults;
                presults.boxes_.emplace_back(bounding_box);
                presults.confidences_.emplace_back(confidence);
                mapper.insert({id, std::move(presults)});
            }
        }
    }    

    return mapper;
}

void ssd_detector::set_class_names(std::vector<string> class_names)
{
    class_names_ = std::move(class_names);
}
