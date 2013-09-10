#include <array>
#include <iostream>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <openCVToQt.hpp>

#include <QFileDialog>
#include <QImage>
#include <QMessageBox>
#include <QStringList>

#include <basicImageAlgo.hpp>
#include <imageAlgorithm.hpp>

#include "histBackProjectUI.hpp"
#include "ui_histbackprojectui.h"

histBackProjectUI::histBackProjectUI(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::histBackProjectUI)
{
    ui->setupUi(this);

    connect(ui->button_load_input_, SIGNAL(clicked()), this, SLOT(load_input()));
    connect(ui->button_load_model_, SIGNAL(clicked()), this, SLOT(load_model()));
    connect(ui->button_run_, SIGNAL(clicked()), this, SLOT(run()));

    ui->combobox_morph->addItems(QStringList{"RECT", "CROSS", "ELLIPSE"});

    input_ = cv::imread("/Users/Qt/program/experiment_apps_and_libs/computerVisionApp/pic/rose_small.png");
    model_ = cv::imread("/Users/Qt/program/experiment_apps_and_libs/computerVisionApp/pic/rose_region.png");
    QImage input = mat_to_qimage_cpy(input_);
    QImage model = mat_to_qimage_cpy(model_);
    ui->label_input_image_->setPixmap(QPixmap::fromImage(input));
    ui->label_model_image_->setPixmap(QPixmap::fromImage(model));
}

histBackProjectUI::~histBackProjectUI()
{
    delete ui;
}

/**********************************************************
 ****************** implementation ************************
 **********************************************************/

int histBackProjectUI::get_morphological_type(QString const& type) const
{
    if(type == "RECT"){
        return cv::MORPH_RECT;
    }
    else if(type == "CROSS"){
        return cv::MORPH_CROSS;
    }
    else if(type == "ELLIPSE"){
        return cv::MORPH_ELLIPSE;
    }

    return cv::MORPH_RECT;
}

void histBackProjectUI::load_input()
{
    load(input_);
    QImage input = mat_to_qimage_cpy(input_);
    ui->label_input_image_->setPixmap(QPixmap::fromImage(input));
}

void histBackProjectUI::load_model()
{
    load(model_);
    QImage model = mat_to_qimage_cpy(model_);
    ui->label_model_image_->setPixmap(QPixmap::fromImage(model));
}

void histBackProjectUI::load(cv::Mat &mat)
{
    QString const fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                          "",
                                                          tr("Images (*.png *.xpm *.jpg)"));

    mat = cv::imread(fileName.toStdString());
    if(mat.empty()){
        QMessageBox::critical(this, "Error", fileName + " do not exist");
        return;
    }
}

void histBackProjectUI::run()
{
    if(input_.empty() || model_.empty()){
        QMessageBox::critical(this, tr("Error"), tr("input or model do not exist"));
        return;
    }

    typedef std::pair<cv::Mat, cv::Mat> Result;

    cv::Size const kernel_size(ui->slider_kernel_width_->value() % 2 == 0 ? ui->slider_kernel_width_->value() + 1 : ui->slider_kernel_width_->value(),
                               ui->slider_kernel_height_->value() % 2 == 0 ? ui->slider_kernel_height_->value() + 1 : ui->slider_kernel_height_->value());
    int const morph_type = get_morphological_type(ui->combobox_morph->currentText());
    impl_.set_input_and_model(input_, model_);
    Result custom_back_project = impl_.custom_back_project(ui->slider_hist_dim_->value(), kernel_size, morph_type);
    Result opencv_back_project = impl_.openCV_back_project(ui->slider_hist_dim_->value(), kernel_size, morph_type);

    QImage map_custom = mat_to_qimage_cpy(custom_back_project.first);
    QImage result_custom = mat_to_qimage_cpy(custom_back_project.second);
    QImage map_opencv = mat_to_qimage_cpy(opencv_back_project.first);
    QImage result_opencv = mat_to_qimage_cpy(opencv_back_project.second);
    ui->label_custom_map_image_->setPixmap(QPixmap::fromImage(map_custom));
    ui->label_custom_result_image_->setPixmap(QPixmap::fromImage(result_custom));
    ui->label_opencv_map_image_->setPixmap(QPixmap::fromImage(map_opencv));
    ui->label_opencv_result_image_->setPixmap(QPixmap::fromImage(result_opencv));
}

