#include "color_filter_widget.hpp"
#include "ui_color_filter_widget.h"

#include <QFileDialog>
#include <QFileInfo>
#include <QImage>
#include <QPixmap>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

namespace{

QImage resize_qimg(QImage const &input)
{
    if(input.width() > input.height()){
        return input.scaledToWidth(480);
    }

    return input.scaledToHeight(320);
}

QImage to_qimage(cv::Mat const &input, QImage::Format format = QImage::Format_RGB888)
{
    QImage qimg(input.data, input.cols, input.rows, static_cast<int>(input.step[0]), format);
    qimg = qimg.rgbSwapped();
    qimg = resize_qimg(qimg);

    return qimg;
}

}

color_filter_widget::color_filter_widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::color_filter_widget)
{
    ui->setupUi(this);
}

color_filter_widget::~color_filter_widget()
{
    delete ui;
}

void color_filter_widget::on_pushButtonSelectImage_clicked()
{
    auto const fname = QFileDialog::getOpenFileName(this, "", last_dir_, tr("Images (*.png *.xpm *.jpg *.jpeg)"));
    origin_img_ = cv::imread(fname.toStdString());

    if(!origin_img_.empty()){
        last_dir_ = QFileInfo(fname).absoluteDir().path();
        ui->labelOriginImage->setPixmap(QPixmap::fromImage(to_qimage(origin_img_)));
        update_hsv_image();
    }
}

void color_filter_widget::on_pushButtonApply_clicked()
{
    update_hsv_image();
}

void color_filter_widget::on_horizontalSliderHueLow_valueChanged(int)
{
    update_hsv_image();
}

void color_filter_widget::on_horizontalSliderHueUp_valueChanged(int)
{
    update_hsv_image();
}

void color_filter_widget::update_hsv_image()
{
    if(!origin_img_.empty()){
        auto const hue_low = ui->spinBoxHueLow->value();
        auto const hue_high = ui->spinBoxHueUp->value();
        auto const sat_low = ui->spinBoxSatLow->value();
        auto const sat_high = ui->spinBoxSatUp->value();
        auto const val_low = ui->spinBoxValLow->value();
        auto const val_high = ui->spinBoxValUp->value();

        cv::Mat hsv;
        cv::GaussianBlur(origin_img_, hsv, cv::Size(7, 7), 0);
        cv::cvtColor(origin_img_, hsv, cv::COLOR_BGR2HSV);
        cv::Mat threshold;
        cv::inRange(hsv, cv::Scalar(hue_low, sat_low, val_low),
                    cv::Scalar(hue_high, sat_high, val_high), threshold);

        ui->labelImageFiltered->setPixmap(QPixmap::fromImage(to_qimage(threshold, QImage::Format_Grayscale8)));
    }
}
