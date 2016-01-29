#ifndef CAR_BENCHMARK_H
#define CAR_BENCHMARK_H

#include <opencv2/core.hpp>

#include <tiny_cnn/tiny_cnn.h>

#include <map>

class car_benchmark
{
public:
    car_benchmark();

private:
    using Imgs = std::vector<tiny_cnn::vec_t>;
    using Labels = std::vector<tiny_cnn::label_t>;

    void add_data(tiny_cnn::label_t label, cv::Mat const &img,
                  Labels &labels, Imgs &imgs);
    void load_car_data(std::string const &folder,
                       std::map<std::string, cv::Rect> const &car_region,
                       Imgs &imgs, Labels &labels);
    std::map<std::string, cv::Rect>
    load_car_region(std::string const &file) const;
    void load_data(std::string const &folder,
                   tiny_cnn::label_t label,
                   Imgs &imgs, Labels &labels);
    void train_test();

    Imgs test_images_;
    Labels test_labels_;
    Imgs train_images_;
    Labels train_labels_;    
};

#endif // CAR_DETECTION_H
