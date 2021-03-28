#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <QString>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

class QWidget;

QWidget* place_to_center_widget(QWidget *target);

cv::Mat read_cv_img(QString const &url,
                    int interpolate = cv::INTER_LINEAR_EXACT,
                    cv::Size const &resize_to = {0, 0});

cv::Mat read_image_by_fstream(std::wstring const &fpath);

#endif // UTILITY_HPP
