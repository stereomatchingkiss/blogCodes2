#ifndef CAR_BENCHMARK_H
#define CAR_BENCHMARK_H

#include <tiny_cnn/tiny_cnn.h>

class car_benchmark
{
public:
    car_benchmark();

private:
    using Imgs = std::vector<tiny_cnn::vec_t>;
    using Labels = std::vector<tiny_cnn::label_t>;

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
