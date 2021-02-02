#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <opencv2/core.hpp>

class QWidget;

QWidget* place_to_center_widget(QWidget *target);

cv::Mat read_image_by_fstream(std::wstring const &fpath);

#endif // UTILITY_HPP
