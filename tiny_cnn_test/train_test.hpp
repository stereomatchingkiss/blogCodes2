#ifndef TRAIN_TEST_HPP
#define TRAIN_TEST_HPP

#include <boost/progress.hpp>

#include <tiny_cnn/tiny_cnn.h>

#include <fstream>
#include <iostream>
#include <vector>

class train_test
{
public:
    train_test() = default;
    train_test(int minibatch_size, int num_epochs) :
        minibatch_size_{minibatch_size},
        num_epochs_{num_epochs}
    {}

    int get_minibatch_size() const
    {
        return minibatch_size_;
    }

    int get_num_epoch() const
    {
        return num_epochs_;
    }

    void set_minibatch_size(int value)
    {
        minibatch_size_ = value;
    }

    void set_num_epoch(int value)
    {
        num_epochs_ = value;
    }

    template<typename Net, typename Img, typename Label>
    void train_and_test(Net &net, std::vector<Img> const &train_img,
                        std::vector<Label> const &train_label,
                        std::vector<Img> const &test_img,
                        std::vector<Label> const &test_label);

private:
    int minibatch_size_ = 1;
    int num_epochs_ = 30;
};

template<typename Net, typename Img, typename Label>
void train_test::train_and_test(Net &net, std::vector<Img> const &train_img,
                                std::vector<Label> const &train_label,
                                std::vector<Img> const &test_img,
                                std::vector<Label> const &test_label)
{
    std::cout << "start learning" << std::endl;
    boost::progress_display disp(static_cast<int>(train_img.size()));
    boost::timer t;
    // create callback
    auto on_enumerate_epoch = [&](){
        std::cout << t.elapsed() << "s elapsed." << std::endl;
        tiny_cnn::result res = nn.test(test_img, test_label);
        std::cout << res.num_success << "/" << res.num_total << std::endl;

        disp.restart(static_cast<int>(train_img.size()));
        t.restart();
    };

    auto on_enumerate_minibatch = [&](){
        disp += minibatch_size_;
    };

    // training
    net.train(train_img, train_label, minibatch_size_, num_epochs_,
              on_enumerate_minibatch, on_enumerate_epoch);

    std::cout << "end training." << std::endl;

    // test and show results
    net.test(test_img, test_label).print_detail(std::cout);

    // save networks
    std::ofstream ofs("car_weights");
    ofs << nn;
}

#endif // TRAIN_TEST_HPP
