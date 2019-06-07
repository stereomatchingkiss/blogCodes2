#include "text_detector_east.hpp"

#include <iostream>

using namespace cv;
using namespace cv::dnn;
using namespace std;

namespace{

void decode(const Mat& scores, const Mat& geometry, float scoreThresh,
            std::vector<RotatedRect>& detections, std::vector<float>& confidences)
{
    CV_Assert(scores.dims == 4);
    CV_Assert(geometry.dims == 4);
    CV_Assert(scores.size[0] == 1);
    CV_Assert(geometry.size[0] == 1);
    CV_Assert(scores.size[1] == 1);
    CV_Assert(geometry.size[1] == 5);
    CV_Assert(scores.size[2] == geometry.size[2]);
    CV_Assert(scores.size[3] == geometry.size[3]);

    const int height = scores.size[2];
    const int width = scores.size[3];
    for (int y = 0; y < height; ++y){
        const float* scoresData = scores.ptr<float>(0, 0, y);
        const float* x0_data = geometry.ptr<float>(0, 0, y);
        const float* x1_data = geometry.ptr<float>(0, 1, y);
        const float* x2_data = geometry.ptr<float>(0, 2, y);
        const float* x3_data = geometry.ptr<float>(0, 3, y);
        const float* anglesData = geometry.ptr<float>(0, 4, y);
        for (int x = 0; x < width; ++x){
            float const score = scoresData[x];
            if(score >= scoreThresh){

                // Decode a prediction.
                // Multiple by 4 because feature maps are 4 time less than input image.
                float offsetX = x * 4.0f, offsetY = y * 4.0f;
                float angle = anglesData[x];
                float cosA = std::cos(angle);
                float sinA = std::sin(angle);
                float h = x0_data[x] + x2_data[x];
                float w = x1_data[x] + x3_data[x];

                Point2f offset(offsetX + cosA * x1_data[x] + sinA * x2_data[x],
                               offsetY - sinA * x1_data[x] + cosA * x2_data[x]);
                Point2f p1 = Point2f(-sinA * h, -cosA * h) + offset;
                Point2f p3 = Point2f(-cosA * w, sinA * w) + offset;
                RotatedRect r(0.5f * (p1 + p3), Size2f(w, h), -angle * 180.0f / static_cast<float>(CV_PI));
                detections.emplace_back(r);
                confidences.emplace_back(score);
            }
        }
    }
}

}

text_detector_east::text_detector_east(std::string const &model,
                                       float confident_threshold,
                                       float nms_threshold,
                                       cv::Size const &process_size) :
    confident_threshold_(confident_threshold),
    net_(readNet(model)),
    nms_threshold_(nms_threshold),
    process_size_(process_size)
{
    out_names_.emplace_back("feature_fusion/Conv_7/Sigmoid");
    out_names_.emplace_back("feature_fusion/concat_3");
}

text_detector_east::result_type text_detector_east::detect(const Mat &input)
{
    blobFromImage(input, blob_, 1.0, process_size_, Scalar(123.68, 116.78, 103.94), true);
    net_.setInput(blob_);
    net_.forward(outputs_, out_names_);

    Mat const &scores = outputs_[0];
    Mat const &geometry = outputs_[1];
    result_type results;
    decode(scores, geometry, confident_threshold_, results.boxes_, results.confidences_);
    NMSBoxes(results.boxes_, results.confidences_, confident_threshold_, nms_threshold_, results.indices_);

    return results;
}
