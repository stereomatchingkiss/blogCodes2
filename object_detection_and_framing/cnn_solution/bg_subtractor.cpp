#include "bg_subtractor.hpp"

#include "bg_subtractor_config.hpp"

#include <opencv2/imgproc.hpp>

using namespace cv;

bg_subtractor::bg_subtractor(const bg_subtractor_config &params) :
    params_(std::make_unique<bg_subtractor_config>(params))
{
    subtractor_ = cv::createBackgroundSubtractorMOG2(params.history_,
                                                     params.var_threshold_, params.detect_shadow_);
}

bg_subtractor::~bg_subtractor()
{

}

bg_subtractor::results bg_subtractor::apply(const Mat &input)
{
    fpack_.input_frame_ = input;
    resize_input_frame();
    subtractor_->apply(fpack_.input_frame_resize_, fpack_.fore_ground_mask_);
    auto const contours = find_contours();

    results res;
    res.foreground_mask_ = fpack_.fore_ground_mask_;
    res.foreground_.setTo(0);
    fpack_.input_frame_resize_.copyTo(res.foreground_, res.foreground_mask_);

    return res;
}

std::vector<std::vector<Point> > bg_subtractor::find_contours()
{
    using namespace std;

    vector<vector<Point>> contours;
    GaussianBlur(fpack_.fore_ground_mask_, fpack_.fore_ground_mask_, params_->gaussian_kernal_size_, 0);
    cv::threshold(fpack_.fore_ground_mask_, fpack_.thresh_, params_->thresh_val_, 255, cv::THRESH_BINARY);
    cv::dilate(fpack_.thresh_, fpack_.thresh_,
               getStructuringElement(MORPH_RECT, params_->dilate_kernal_size_),
               Point(-1, -1), 2);
    findContours(fpack_.thresh_, contours, {}, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    return contours;
}

void bg_subtractor::resize_input_frame()
{
    if(fpack_.input_frame_resize_.empty()){
        if(params_->resize_width_ != 0){
            double const resize_scale = params_->resize_width_/static_cast<double>(fpack_.input_frame_.cols);
            params_->resize_to_ = Size(params_->resize_width_,
                                       static_cast<int>(fpack_.input_frame_.rows * resize_scale));
        }else{
            params_->resize_to_ = Size(fpack_.input_frame_.cols, fpack_.input_frame_.rows);
        }
    }
    cv::resize(fpack_.input_frame_, fpack_.input_frame_resize_, params_->resize_to_);
}
