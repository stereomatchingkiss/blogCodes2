#ifndef OPEN_IMAGE_DETECTOR_INFO_HPP
#define OPEN_IMAGE_DETECTOR_INFO_HPP

#include <QString>

struct open_image_detector_info{
    QString label_;
    float xmin_;
    float xmax_;
    float ymin_;
    float ymax_;
};

#endif // OPEN_IMAGE_DETECTOR_INFO_HPP
