#ifndef TINY_CNN_TRAINER_HPP
#define TINY_CNN_TRAINER_HPP

#include <tiny_cnn/tiny_cnn.h>

class tiny_cnn_tainer
{
public:
    void train();

private:
    using Imgs = std::vector<tiny_cnn::vec_t>;
    using Labels = std::vector<tiny_cnn::label_t>;

    Imgs test_images_;
    Labels test_labels_;
    Imgs train_images_;
    Labels train_labels_;
};

#endif //TINY_CNN_TRAINER_HPP
