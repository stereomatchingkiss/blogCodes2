#include "saliency_image_generator.hpp"

#include <net.h>

#include <opencv2/imgproc.hpp>

struct saliency_image_generator::pimpl
{
    pimpl(std::string const &param_path, std::string const &bin_path)
    {
        net_.opt.use_vulkan_compute = false;
        net_.opt.use_fp16_arithmetic = false;
        load_param_success_ = net_.load_param(param_path.c_str()) == 0;
        load_model_success_ = net_.load_model(bin_path.c_str()) == 0;
    }

    cv::Mat preprocess(cv::Mat const &input)
    {
        cv::Mat resize_img;
        cv::resize(input, resize_img, cv::Size(320, 320));
        cv::cvtColor(resize_img, resize_img, cv::COLOR_BGR2RGB);

        return resize_img;
    }

    float max_value_of_mat(cv::Mat const &input) const noexcept
    {
        auto *ptr = input.ptr<uchar>();
        auto it = std::max_element(ptr, ptr + input.total() * input.channels());

        return std::max(1.0f, static_cast<float>(*it));
    }

    void divide_by_value(ncnn::Mat &inout_tensor, float max_value)
    {
        for(int i = 0; i != inout_tensor.h; ++i){
            auto *ptr = inout_tensor.row(i);
            for(int j = 0; j != inout_tensor.w; ++j){
                ptr[j] /= max_value;
            }
        }
    }

    cv::Mat generate(cv::Mat const &input)
    {
        auto resize_img = preprocess(input);
        auto const max_value = max_value_of_mat(resize_img);        
        ncnn::Mat input_tensor = ncnn::Mat::from_pixels(resize_img.data, ncnn::Mat::PIXEL_RGB, resize_img.rows, resize_img.rows);
        divide_by_value(input_tensor, max_value);
        float constexpr mean_vals[] = {123.675f, 116.28f, 103.53f};
        float constexpr norm_vals[] = {0.017125f, 0.017507f, 0.017429f};
        input_tensor.substract_mean_normalize(mean_vals, norm_vals);

        auto ex = net_.create_extractor();
        ex.set_light_mode(false);
        ex.set_num_threads(4);
        ex.input("input.1", input_tensor);
        ncnn::Mat output;
        ex.extract(net_.output_names()[0], output);        
        cv::Mat results(320, 320, CV_32F);
        auto *pix = static_cast<float*>(output.data);
        std::copy(pix, pix + output.total(), results.ptr<float>());
        cv::normalize(results, results, cv::NORM_MINMAX);
        results *= 255;
        cv::resize(results, results, cv::Size(input.cols, input.rows));

        return results;
    }

    std::vector<cv::Rect> extract_bboxes(cv::Mat const &input)
    {
        using namespace std;
        using namespace cv;

        vector<vector<Point> > contours;
        vector<Vec4i> hierarchy;
        findContours(input, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);

        vector<Rect> bboxes;
        for(size_t i = 0; i< contours.size(); ++i){
            auto const bbox = boundingRect(contours[i]);
            bboxes.emplace_back(bbox);
        }
        std::sort(std::begin(bboxes), std::end(bboxes), [](auto const &lhs, auto const &rhs)
        {
            return lhs.area() > rhs.area();
        });

        return bboxes;
    }

    std::vector<cv::Rect> generate_bboxes(const cv::Mat &input)
    {
        auto otsu_mask = generate_otsu_mask(input);
        return extract_bboxes(otsu_mask);
    }

    std::tuple<std::vector<cv::Rect>, cv::Mat> generate_bbox_and_image_after_mask(cv::Mat const &input)
    {
        auto otsu_mask = generate_otsu_mask(input);
        auto bboxes = extract_bboxes(otsu_mask);

        return {std::move(bboxes), otsu_mask};
    }

    cv::Mat generate_image_after_mask(cv::Mat const &input)
    {
        auto mask = generate_otsu_mask(input);
        cv::Mat output;
        cv::cvtColor(mask, output, cv::COLOR_GRAY2BGR);
        cv::bitwise_and(output, input, output);

        return output;
    }

    cv::Mat generate_otsu_mask(cv::Mat const &input)
    {
        auto mask = generate(input);
        mask.convertTo(mask, CV_8U);
        cv::threshold(mask, mask, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);

        return mask;
    }

    ncnn::Mat input_tensor_;
    ncnn::Net net_;

    bool load_model_success_ = false;
    bool load_param_success_ = false;
};

saliency_image_generator::saliency_image_generator(std::string const &param_path, std::string const &bin_path) :
    impl_(std::make_unique<pimpl>(param_path, bin_path))
{

}

saliency_image_generator::~saliency_image_generator()
{

}

std::vector<cv::Rect> saliency_image_generator::extract_bboxes(const cv::Mat &input)
{
    return impl_->extract_bboxes(input);
}

bool saliency_image_generator::get_load_model_success() const noexcept
{
    return impl_->load_model_success_;
}

bool saliency_image_generator::get_load_param_success() const noexcept
{
    return impl_->load_param_success_;
}

cv::Mat saliency_image_generator::generate(cv::Mat const &input)
{
    return impl_->generate(input);
}

std::vector<cv::Rect> saliency_image_generator::generate_bboxes(const cv::Mat &input)
{
    return impl_->generate_bboxes(input);
}

std::tuple<std::vector<cv::Rect>, cv::Mat> saliency_image_generator::generate_bbox_and_image_after_mask(const cv::Mat &input)
{
    return impl_->generate_bbox_and_image_after_mask(input);
}

cv::Mat saliency_image_generator::generate_image_after_mask(const cv::Mat &input)
{
    return impl_->generate_image_after_mask(input);
}

cv::Mat saliency_image_generator::generate_otsu_mask(cv::Mat const &input)
{
    return impl_->generate_otsu_mask(input);
}
