#include "yolov3_detector.hpp"

using namespace cv;
using namespace cv::dnn;
using namespace std;

namespace{

std::vector<std::string> create_coco_obj_detection_labels()
{
    return {
        "person", "bicycle", "car", "motorcycle", "airplane", "bus", "train",
        "truck", "boat", "traffic light", "fire hydrant", "stop sign",
        "parking meter", "bench", "bird", "cat", "dog", "horse", "sheep",
        "cow", "elephant", "bear", "zebra", "giraffe", "backpack", "umbrella",
        "handbag", "tie", "suitcase", "frisbee", "skis", "snowboard",
        "sports ball", "kite", "baseball bat", "baseball glove", "skateboard",
        "surfboard", "tennis racket", "bottle", "wine glass", "cup", "fork",
        "knife", "spoon", "bowl", "banana", "apple", "sandwich", "orange",
        "broccoli", "carrot", "hot dog", "pizza", "donut", "cake", "chair",
        "couch", "potted plant", "bed", "dining table", "toilet", "tv",
        "laptop", "mouse", "remote", "keyboard", "cell phone", "microwave",
        "oven", "toaster", "sink", "refrigerator", "book", "clock", "vase",
        "scissors", "teddy bear", "hair drier", "toothbrush"
    };
}

}

yolov3_detector::yolov3_detector(const std::string &model_bin,
                                 const std::string &proto_txt,
                                 float confidence,
                                 const Size &process_size,
                                 float nms_threshold) :
    confidence_(confidence),
    nms_threshold_(nms_threshold),
    process_size_(process_size)
{
    net_ = readNetFromDarknet(proto_txt, model_bin);
    net_.setPreferableBackend(DNN_BACKEND_OPENCV);
    net_.setPreferableTarget(DNN_TARGET_CPU);

    class_names_ = create_coco_obj_detection_labels();

    init_output_names();
}

// Remove the bounding boxes with low confidence using non-maxima suppression
yolov3_detector::result_type
yolov3_detector::extract_forward_results(cv::Size const &img_size, vector<Mat> const &outs) const
{
    predict_resutls presults;
    std::vector<int> class_ids;
    for(size_t i = 0; i < outs.size(); ++i){
        auto *data = reinterpret_cast<float*>(outs[i].data);
        for(int j = 0; j < outs[i].rows; ++j, data += outs[i].cols){
            Mat const scores = outs[i].row(j).colRange(5, outs[i].cols);
            Point class_id_point;
            double confidence;
            minMaxLoc(scores, nullptr, &confidence, nullptr, &class_id_point);
            if(static_cast<float>(confidence) > confidence_){
                int const centerX = static_cast<int>(data[0] * img_size.width);
                int const centerY = static_cast<int>(data[1] * img_size.height);
                int const width = static_cast<int>(data[2] * img_size.width);
                int const height = static_cast<int>(data[3] * img_size.height);
                int const left = centerX - width / 2;
                int const top = centerY - height / 2;

                class_ids.emplace_back(class_id_point.x);
                presults.confidences_.emplace_back(static_cast<float>(confidence));
                presults.boxes_.emplace_back(Rect(left, top, width, height));
            }
        }
    }

    return nonmaxima_suppress(presults, class_ids);
}

yolov3_detector::result_type yolov3_detector::detect(const Mat &input)
{
    blobFromImage(input, buffer_, 1/255.0, process_size_, {0, 0, 0}, true);
    net_.setInput(buffer_);

    vector<Mat> outs;
    net_.forward(outs, output_names_);

    return extract_forward_results(Size(input.cols, input.rows), outs);
}

string const&  yolov3_detector::get_name(coco_item_type input) const noexcept
{
    return class_names_[static_cast<size_t>(input)];
}

void yolov3_detector::init_output_names()
{
    vector<int> const out_layers = net_.getUnconnectedOutLayers();
    vector<String> const layer_names = net_.getLayerNames();
    output_names_.resize(out_layers.size());
    for (size_t i = 0; i < out_layers.size(); ++i){
        output_names_[i] = layer_names[static_cast<size_t>(out_layers[i] - 1)];
    }
}

yolov3_detector::result_type yolov3_detector::nonmaxima_suppress(yolov3_detector::predict_resutls const &input,
                                                                 std::vector<int> const &class_ids) const
{
    vector<int> indices;
    NMSBoxes(input.boxes_, input.confidences_, confidence_, nms_threshold_, indices);
    yolov3_detector::result_type results;
    for (size_t i = 0; i < indices.size(); ++i){
        auto const index = static_cast<size_t>(indices[i]);
        auto const id = static_cast<coco_item_type>(class_ids[index]);
        auto it = results.find(id);
        if(it != std::end(results)){
            it->second.boxes_.emplace_back(input.boxes_[index]);
            it->second.confidences_.emplace_back(input.confidences_[index]);
        }else{
            predict_resutls presults;
            presults.boxes_.emplace_back(input.boxes_[index]);
            presults.confidences_.emplace_back(input.confidences_[index]);
            results.insert({static_cast<coco_item_type>(class_ids[index]), std::move(presults)});
        }
    }

    return results;
}
