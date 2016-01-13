#ifndef FACE_RECOGNITION_HPP
#define FACE_RECOGNITION_HPP

#include <opencv2/core.hpp>
#include <opencv2/face.hpp>

#include <boost/bimap.hpp>

#include <string>

class face_recognition
{
public:
    explicit face_recognition(std::string input_folder,
                              size_t min_train_data,
                              cv::Size const &face_size ={125,125});

    double get_confident() const;
    std::string recognize(cv::Mat const &input);    

private:
    void buid_train_data();
    size_t minimum_train_data();

    boost::bimap<std::string, int> bimap_;
    double confident_ = 0;
    cv::Mat face_;
    cv::Size face_size_;
    std::vector<cv::Mat> images_;
    std::string input_folder_;
    std::vector<int> labels_;
    size_t min_train_data_;
    cv::Ptr<cv::face::LBPHFaceRecognizer> model_;
};

#endif
