#include "utility.hpp"

#include <QCheckBox>
#include <QHBoxLayout>

#include <QDebug>
#include <QFile>

#include <opencv2/imgcodecs.hpp>

#include <fstream>

QWidget *place_to_center_widget(QWidget *target)
{
    QWidget *widget = new QWidget;
    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(target);
    layout->setAlignment(Qt::AlignCenter);
    layout->setContentsMargins(0,0,0,0);
    widget->setLayout(layout);

    return widget;
}

cv::Mat read_image_by_fstream(const std::wstring &fpath)
{
    std::ifstream input(fpath, std::ios::binary);
    if(input.is_open()){

        input.seekg(0, std::ios::end);
        size_t const fileSize = static_cast<size_t>(input.tellg());
        input.seekg(0, std::ios::beg);

        if(fileSize == 0){
            return cv::Mat();
        }

        std::vector<unsigned char> data(fileSize);
        input.read(reinterpret_cast<char*>(&data[0]), static_cast<std::streamsize>(sizeof(unsigned char) * fileSize));

        if(!input){
            return cv::Mat();
        }

        return cv::imdecode(cv::Mat(data), cv::IMREAD_COLOR);
    }

    return {};
}

cv::Mat read_cv_img(const QString &url, int interpolate, const cv::Size &resize_to)
{
    QFile file(url);
    if(file.open(QIODevice::ReadOnly)){
        auto buffer = file.readAll();
        auto cimg = cv::imdecode(cv::Mat(1, buffer.size(), CV_8U, buffer.data()), cv::IMREAD_UNCHANGED);
        if(!cimg.empty()){
            if((resize_to.height * resize_to.width) == 0){
                if(cimg.cols > cimg.rows){
                    if(cimg.cols > 640){
                        cv::resize(cimg, cimg, cv::Size(640, static_cast<int>(640.0/cimg.cols * cimg.rows)), 0, 0,
                                   interpolate);
                    }
                }else{
                    if(cimg.rows > 480){
                        cv::resize(cimg, cimg, cv::Size(static_cast<int>(480.0/cimg.rows * cimg.cols), 480), 0, 0,
                                   interpolate);
                    }
                }
            }else{
                cv::resize(cimg, cimg, resize_to, 0, 0, interpolate);
            }

            return cimg;
        }else{
            qDebug()<<QString("Cannot read image %1").arg(url);
            return cv::Mat();
        }
    }else{
        qDebug()<<QString("Cannot read image %1").arg(url);
        return cv::Mat();
    }
}
