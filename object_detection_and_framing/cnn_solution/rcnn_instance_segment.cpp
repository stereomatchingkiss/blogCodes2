#include "rcnn_instance_segment.hpp"

#include "names_creator.hpp"

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <iostream>

using namespace cv;
using namespace cv::dnn;
using namespace std;

rcnn_instance_segment::rcnn_instance_segment(const std::string &model_bin,
                                             const std::string &proto_txt,
                                             float confidence,
                                             float mask_threahold) :
    class_names_(create_coco_obj_detection_labels()),
    confidence_(confidence),
    mask_threshold_(mask_threahold),
    output_names_{"detection_out_final", "detection_masks"}
{
    net_ = readNetFromTensorflow(model_bin, proto_txt);
    net_.setPreferableBackend(DNN_BACKEND_OPENCV);
    net_.setPreferableTarget(DNN_TARGET_CPU);

    colors_.emplace_back(0, 255, 0, 255.0);
    colors_.emplace_back(0, 0, 255, 255.0);
    colors_.emplace_back(255, 0, 0, 255.0);
    colors_.emplace_back(0, 255, 255, 255.0);
    colors_.emplace_back(255, 255, 0, 255.0);
    colors_.emplace_back(255, 0, 255, 255.0);
    colors_.emplace_back(80, 70, 180, 255.0);
    colors_.emplace_back(250, 80, 190, 255.0);
    colors_.emplace_back(245, 145, 50, 255.0);
    colors_.emplace_back(70, 150, 250, 255.0);
    colors_.emplace_back(50, 190, 190, 255.0);
}

void rcnn_instance_segment::detect(Mat &inout)
{
    // Create a 4D blob from a frame.
    blobFromImage(inout, buffer_, 1.0, Size(inout.cols, inout.rows), {}, true, false);

    //Sets the input to the network
    net_.setInput(buffer_);

    // Runs the forward pass to get output from the output layers
    vector<Mat> outs;
    net_.forward(outs, output_names_);

    // Extract the bounding box and mask for each of the detected objects
    post_process(outs, inout);
}

const string &rcnn_instance_segment::get_name(coco_item_type input) const noexcept
{
    return class_names_[static_cast<size_t>(input)];
}

void rcnn_instance_segment::draw_box(int class_id, float conf, Rect &box, Mat &object_mask, Mat &inout)
{
    //Draw a rectangle displaying the bounding box
    rectangle(inout, Point(box.x, box.y), Point(box.x+box.width, box.y+box.height), Scalar(255, 178, 50), 3);

    //Get the label for the class name and its confidence
    string label = format("%.2f", static_cast<double>(conf));
    if(!class_names_.empty()){
        CV_Assert(class_id < static_cast<int>(class_names_.size()));
        label = class_names_[static_cast<size_t>(class_id)] + ":" + label;
    }

    //Display the label at the top of the bounding box
    int baseLine;
    Size const labelSize = getTextSize(label, FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseLine);
    box.y = max(box.y, labelSize.height);
    rectangle(inout,
              Point(box.x, static_cast<int>(box.y - round(1.5*labelSize.height))),
              Point(static_cast<int>(box.x + round(1.5*labelSize.width)), box.y + baseLine),
              Scalar(255, 255, 255),
              FILLED);
    putText(inout, label, Point(box.x, box.y), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(0,0,0),1);

    Scalar color = colors_[static_cast<size_t>(class_id % static_cast<int>(colors_.size()))];
    // Comment the above line and uncomment the two lines below to generate different instance colors
    //int colorInd = rand() % colors.size();
    //Scalar color = colors[colorInd];

    // Resize the mask, threshold, color and apply it on the image
    resize(object_mask, object_mask, Size(box.width, box.height));
    Mat mask = (object_mask > static_cast<double>(mask_threshold_));
    Mat coloredRoi = (0.3 * color + 0.7 * inout(box));
    coloredRoi.convertTo(coloredRoi, CV_8UC3);

    // Draw the contours on the image
    vector<Mat> contours;
    Mat hierarchy;
    mask.convertTo(mask, CV_8U);
    findContours(mask, contours, hierarchy, RETR_CCOMP, CHAIN_APPROX_SIMPLE);
    drawContours(coloredRoi, contours, -1, color, 5, LINE_8, hierarchy, 100);
    coloredRoi.copyTo(inout(box), mask);
}

void rcnn_instance_segment::post_process(std::vector<Mat> const &predict_results, Mat &inout)
{
    Mat out_detections = predict_results[0];
    Mat out_masks = predict_results[1];

    // Output size of masks is NxCxHxW where
    // N - number of detected boxes
    // C - number of classes (excluding background)
    // HxW - segmentation shape
    auto const num_detections = out_detections.size[2];
    //auto const numClasses = out_masks.size[1];
    inout.setTo(0);
    if(num_detections == 0){
        return;
    }

    out_detections = out_detections.reshape(1, static_cast<int>(out_detections.total() / 7));
    for(int i = 0; i < num_detections; ++i){
        float score = out_detections.at<float>(i, 2);
        if (score > confidence_){

            // Extract the bounding box
            int const class_id = static_cast<int>(out_detections.at<float>(i, 1));
            if(class_id == static_cast<int>(coco_item_type::person)){
                int left = static_cast<int>(inout.cols * out_detections.at<float>(i, 3));
                int top = static_cast<int>(inout.rows * out_detections.at<float>(i, 4));
                int right = static_cast<int>(inout.cols * out_detections.at<float>(i, 5));
                int bottom = static_cast<int>(inout.rows * out_detections.at<float>(i, 6));

                left = max(0, min(left, inout.cols - 1));
                top = max(0, min(top, inout.rows - 1));
                right = max(0, min(right, inout.cols - 1));
                bottom = max(0, min(bottom, inout.rows - 1));
                Rect box = Rect(left, top, right - left + 1, bottom - top + 1);

                // Extract the mask for the object
                Mat object_mask(out_masks.size[2], out_masks.size[3],CV_32F, out_masks.ptr<float>(i,class_id));

                // Draw bounding box, colorize and show the mask on the image
                draw_box(class_id, score, box, object_mask, inout);
            }
        }
    }
}
