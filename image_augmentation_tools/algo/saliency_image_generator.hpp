#ifndef SALIENCY_IMAGE_GENERATOR_HPP
#define SALIENCY_IMAGE_GENERATOR_HPP

#include <opencv2/core.hpp>

#include <memory>
#include <string>

class saliency_image_generator
{
public:
    saliency_image_generator(std::string const &param_path, std::string const &bin_path);
    ~saliency_image_generator();

    std::vector<cv::Rect> extract_bboxes(cv::Mat const &input);

    bool get_load_model_success() const noexcept;
    bool get_load_param_success() const noexcept;

    cv::Mat generate(cv::Mat const &input);
    std::vector<cv::Rect> generate_bboxes(cv::Mat const &input);
    std::tuple<std::vector<cv::Rect>, cv::Mat> generate_bbox_and_image_after_mask(cv::Mat const &input);
    cv::Mat generate_image_after_mask(cv::Mat const &input);
    cv::Mat generate_otsu_mask(cv::Mat const &input);

private:
    struct pimpl;

    std::unique_ptr<pimpl> impl_;
};

#endif // SALIENCY_IMAGE_GENERATOR_HPP
