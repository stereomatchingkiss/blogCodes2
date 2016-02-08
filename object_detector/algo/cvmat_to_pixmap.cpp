#include "cvmat_to_pixmap.hpp"

#include <ocv_libs/core/resize.hpp>
#include <ocv_libs/qt/mat_and_qimage.hpp>

#include <opencv2/highgui.hpp>

#include <QDebug>

#include <iostream>

cvmat_to_pixmap::cvmat_to_pixmap(QSize const &size) :
    size_(size)
{

}

QPixmap cvmat_to_pixmap::to_pixmap(cv::Mat input)
{
    if(!input.empty()){
        if(input.rows > size_.height() ||
                input.cols > size_.width()){
            qDebug()<<this<<" : "<<size_;
            auto const new_size = input.cols > input.rows ?
                        cv::Size(size_.width(), 0) :
                        cv::Size(0, size_.height());

            ocv::resize_aspect_ratio(input, img_, new_size);
            //std::cout<<"resize img size : "<<img_.size()<<std::endl;
        }else{
            img_ = input;
        }

        return QPixmap::fromImage(ocv::qt::mat_to_qimage_ref(img_));
    }

    return {};
}

QPixmap cvmat_to_pixmap::to_pixmap(const QString &name)
{
    auto img = cv::imread(name.toStdString());

    return to_pixmap(img);
}
