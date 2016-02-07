#include "car_benchmark.hpp"
#include "mnist_benchmark.hpp"

#include <ocv_libs/cmd/command_prompt_utility.hpp>

#include <boost/filesystem.hpp>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

#include <fstream>
#include <random>

void split_train_test();

int main(int argc, char *argv[])
{
    /*using namespace ocv::cmd;

    auto const map = default_command_line_parser(argc, argv).first;
    if(map.count("image")){
        auto const name = map["image"].as<std::string>();
        auto img = cv::imread(name, CV_LOAD_IMAGE_GRAYSCALE);
        car_benchmark cb;
        auto const result = cb.predict(img);
        for(auto const &var : result){
            std::cout<<var.first<<", "<<var.second<<std::endl;
        }
    }else{
        std::cout<<"must specify image location"<<std::endl;
        return -1;
    }*/

    //split_train_test();
    car_benchmark().train();
    //mnist_benchmark();
}

void split_train_test()
{
    std::ifstream in("oc256.txt");
    //std::ifstream in("oc256.txt");

    std::string str;
    std::vector<std::string> names;
    while(std::getline(in, str)){
        names.emplace_back(str);
    }
    std::random_device rd;
    std::default_random_engine g(rd());
    std::shuffle(std::begin(names), std::end(names), g);
    for(size_t i = 0; i != 8144; ++i){
        boost::filesystem::path path(names[i]);
        boost::filesystem::copy_file(names[i],
                                     "train_non_cars/" + path.filename().generic_string());
    }
    for(size_t i = 8144; i != 9144; ++i){
        boost::filesystem::path path(names[i]);
        boost::filesystem::copy_file(names[i],
                                     "test_non_cars/" + path.filename().generic_string());
    }
}
