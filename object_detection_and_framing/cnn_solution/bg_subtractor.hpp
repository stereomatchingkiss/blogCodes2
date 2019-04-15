#ifndef BG_SUBTRACTOR_HPP
#define BG_SUBTRACTOR_HPP

#include <opencv2/core.hpp>
#include <opencv2/video/background_segm.hpp>

#include <memory>
#include <vector>

struct bg_subtractor_config;

class bg_subtractor
{
public:
    struct config
    {
        bool detect_shadow_ = false;
        cv::Size dilate_kernal_size_ = cv::Size(3, 3);
        cv::Size gaussian_kernal_size_ = cv::Size(21, 21);
        int history_ = 60;
        double min_countour_area_ = 8000;
        cv::Size resize_to_;
        int resize_width_ = 0;
        double thresh_val_ = 30;
        double var_threshold_ = 32;
        int wait_between_frame_ms_ = 1;
    };

    struct results
    {
        cv::Mat foreground_;
        cv::Mat foreground_mask_;
    };

    explicit bg_subtractor(bg_subtractor_config const &params);
    ~bg_subtractor();

    results apply(cv::Mat const &input);

private:
    struct frame_pack
    {
        cv::Mat fore_ground_mask_;
        cv::Mat frame_delta_;
        cv::Mat input_frame_;
        cv::Mat input_frame_gray_;
        cv::Mat input_frame_resize_;
        cv::Mat thresh_;
    };

    std::vector<std::vector<cv::Point>> find_contours();
    void resize_input_frame();

    frame_pack fpack_;
    std::unique_ptr<bg_subtractor_config> params_;
    cv::Ptr<cv::BackgroundSubtractor> subtractor_;
};

#endif // BG_SUBTRACTOR_HPP
