#ifndef OPENCVTOQT_HPP
#define OPENCVTOQT_HPP

/**
 * collect some global functions to cross the gaps
 * between openCV and QImage
 */

class QImage;

namespace cv{

class Mat;

}

QImage mat_to_qimage_cpy(cv::Mat const &mat, bool swap = true);

QImage mat_to_qimage_ref(cv::Mat &mat, bool swap = true);

cv::Mat qimage_to_mat_cpy(QImage const &img, bool swap = true);

cv::Mat qimage_to_mat_ref(QImage &img, bool swap = true);

#endif // OPENCVTOQT_HPP
