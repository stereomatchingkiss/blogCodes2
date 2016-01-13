#include "face_recognition.hpp"

#include <ocv_libs/file/utility.hpp>

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <boost/filesystem.hpp>

#include <algorithm>
#include <iostream>
#include <random>

void face_recognition::buid_train_data()
{
    size_t const min_size = minimum_train_data();
    auto const folders =
            ocv::file::get_directory_folders(input_folder_);
    std::random_device rd;
    std::mt19937 g(rd());
    for(size_t i = 0; i != folders.size(); ++i){
        auto const folder = input_folder_ + "/" + folders[i];
        std::cout<<folder<<std::endl;
        bimap_.insert({folders[i], static_cast<int>(i)});
        auto files = ocv::file::get_directory_files(folder);
        std::shuffle(std::begin(files), std::end(files), g);
        if(min_size != 0){
            files.resize(min_size);
        }
        for(size_t j = 0; j != files.size(); ++j){
            auto img = cv::imread(folder + "/" + files[j]);
            if(!img.empty()){
                cv::cvtColor(img, img, CV_BGR2GRAY);
                cv::resize(img, img, {300, 200});
                images_.emplace_back(img);
                auto it = bimap_.left.find(folders[i]);
                if(it != std::end(bimap_.left)){
                    labels_.emplace_back(it->second);
                }else{
                    throw std::runtime_error("Cannot find folder label");
                }
            }
        }
    }
    std::cout<<images_.size()<<", "<<labels_.size()<<std::endl;
}

size_t face_recognition::minimum_train_data()
{
    auto const folders =
            ocv::file::get_directory_folders(input_folder_);
    size_t min_data = std::numeric_limits<size_t>::max();
    for(auto const &folder : folders){
        auto const dir = input_folder_ + "/" + folder;
        size_t const size =
                ocv::file::get_directory_files(dir).size();
        if(min_data > size){
            min_data = size;
        }
    }

    min_train_data_ = std::min(min_data, min_train_data_);

    return min_train_data_;
}

face_recognition::face_recognition(std::string input_folder,
                                   size_t min_train_data)
    : input_folder_(std::move(input_folder)),
      min_train_data_(min_train_data),
      model_(cv::face::createLBPHFaceRecognizer())
{
    if(!boost::filesystem::exists(input_folder_)){
        throw std::runtime_error("Cannot open input folder");
    }

    buid_train_data();
    model_->train(images_, labels_);
    model_->setThreshold(300);
}

std::string face_recognition::recognize(const cv::Mat &input)
{
    cv::resize(input, face_, {300,200});
    int const label = model_->predict(face_);
    auto it = bimap_.right.find(label);
    if(it != std::end(bimap_.right)){
        return it->second;
    }

    return {};
}
