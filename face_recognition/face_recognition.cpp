#include "face_recognition.hpp"

#include <ocv_libs/file/utility.hpp>

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <boost/filesystem.hpp>

#include <algorithm>
#include <iostream>
#include <random>

face_recognition::face_recognition(std::string input_folder,
                                   size_t min_train_data,
                                   std::string const &face_mode,
                                   cv::Size const &face_size)
    : face_size_(face_size),
      input_folder_(std::move(input_folder)),
      min_train_data_(min_train_data)
{
    if(face_mode == "LBPH"){
        model_ = cv::face::createLBPHFaceRecognizer();
    }else if(face_mode == "EIGEN"){
        model_ = cv::face::createEigenFaceRecognizer();
    }else{
        model_ = cv::face::createFisherFaceRecognizer();
    }

    if(!boost::filesystem::exists(input_folder_)){
        throw std::runtime_error("Cannot open input folder");
    }

    buid_train_data();
    if(!images_.empty() && !labels_.empty()){
        model_->train(images_, labels_);
    }else{
        std::cout<<"Minimum training image should "
                   "not less than 1"<<std::endl;
    }
}

double face_recognition::get_confident() const
{
    return confident_;
}

void face_recognition::buid_train_data()
{
    size_t const min_size = minimum_train_data();
    std::cout<<"minimum size = "<<min_size<<std::endl;
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
            auto img = cv::imread(folder + "/" + files[j],
                                  cv::IMREAD_GRAYSCALE);
            if(!img.empty()){
                if(face_size_.height > 0 && face_size_.width > 0){
                    cv::resize(img, img, face_size_);
                }
                images_.emplace_back(img);
                labels_.emplace_back(static_cast<int>(i));
            }
        }
    }
    std::cout<<images_.size()<<", "<<labels_.size()<<std::endl;
}

size_t face_recognition::minimum_train_data()
{    
    if(min_train_data_ != 0){
        size_t const min_data = ocv::file::get_minimum_file_size(input_folder_);
        std::cout<<"min data : "<<min_data<<std::endl;
        min_train_data_ = std::min(min_data, min_train_data_);
    }

    return min_train_data_;
}

std::string face_recognition::recognize(const cv::Mat &input)
{    
    if(face_size_.height > 0 && face_size_.width > 0){
        cv::resize(input, face_, face_size_);
    }else{
        input.copyTo(face_);
    }
    if(input.type() == CV_8UC3){
        cv::cvtColor(face_, face_, CV_BGR2GRAY);
    }
    int predict_label = -1;
    confident_ = 0;
    model_->predict(face_,predict_label,confident_);
    auto it = bimap_.right.find(predict_label);
    if(it != std::end(bimap_.right)){
        std::cout<<"success confident : "<<confident_<<std::endl;
        return it->second;
    }
    std::cout<<"fail confident : "<<confident_<<std::endl;

    return {};
}
