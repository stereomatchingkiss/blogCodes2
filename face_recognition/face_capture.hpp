#ifndef FACE_CAPTURE_HPP
#define FACE_CAPTURE_HPP

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

#include <string>

class face_capture
{
public:
    explicit face_capture(std::string output_folder);

    int capture();

private:
    int check_validation() const;

    cv::VideoCapture cap_;
    cv::Mat draw_;
    cv::Mat frame_;
    std::string output_folder_;

};

#endif
