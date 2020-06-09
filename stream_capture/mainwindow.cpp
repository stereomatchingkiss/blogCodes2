#include "mainwindow.hpp"
#include "ui_mainwindow.h"

#include "frame_capture_config.hpp"
#include "frame_capture_controller.hpp"

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

#include <QDebug>
#include <QThread>

using namespace frame_capture;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    frame_capture_config config;
    frame_capture_ = new capture_controller(config);
    frame_capture_->add_image_listener([this](cv::Mat input)
    {
        cv::cvtColor(input, input, cv::COLOR_BGR2RGB);
        QImage img(input.data, input.cols, input.rows, static_cast<int>(input.step[0]), QImage::Format_RGB888);
        emit display_image(img.copy());
    }, this);

    connect(this, &MainWindow::display_image, this, &MainWindow::display_image_on_label);

    ui->setupUi(this);

    qDebug()<<__func__<<": main thread = "<<QThread::currentThread();
}

MainWindow::~MainWindow()
{        
    delete  frame_capture_;

    delete ui;
}

void MainWindow::on_pushButtonPlay_clicked()
{
    qDebug()<<__func__;
    frame_capture_->start_url(ui->lineEditUrl->text());
}

void MainWindow::display_image_on_label(QImage input)
{
    ui->labelImage->setPixmap(QPixmap::fromImage(input));
}
