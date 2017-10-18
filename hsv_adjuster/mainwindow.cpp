#include "mainwindow.hpp"
#include "ui_mainwindow.h"

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButtonOpen_clicked()
{
    input_ = cv::imread(ui->lineEditImageAt->text().toStdString());
    QImage img(input_.data, input_.cols, input_.rows, static_cast<int>(input_.step), QImage::Format_RGB888);
    ui->labelImgOrigin->setPixmap(QPixmap::fromImage(img));
    update_hsv_img();
}

void MainWindow::on_horizontalSliderHueLow_valueChanged(int)
{
    update_hsv_img();
}

void MainWindow::on_horizontalSliderSatLow_valueChanged(int)
{
    update_hsv_img();
}

void MainWindow::on_horizontalSliderValLow_valueChanged(int)
{
    update_hsv_img();
}

void MainWindow::on_horizontalSliderHueHigh_valueChanged(int)
{
    update_hsv_img();
}

void MainWindow::on_horizontalSliderSatHigh_valueChanged(int)
{
    update_hsv_img();
}

void MainWindow::on_horizontalSliderValHigh_valueChanged(int)
{
    update_hsv_img();
}

void MainWindow::update_hsv_img()
{
    cv::cvtColor(input_, hsv_, CV_BGR2HSV);
    auto const hrange = std::minmax(ui->horizontalSliderHueLow->value(), ui->horizontalSliderHueHigh->value());
    auto const srange = std::minmax(ui->horizontalSliderSatLow->value(), ui->horizontalSliderSatHigh->value());
    auto const vrange = std::minmax(ui->horizontalSliderValLow->value(), ui->horizontalSliderValHigh->value());
    cv::inRange(hsv_,
                cv::Scalar(hrange.first, srange.first, vrange.first),
                cv::Scalar(hrange.second, srange.second, vrange.second),
                hsv_);
    QImage img(hsv_.data, hsv_.cols, hsv_.rows, static_cast<int>(hsv_.step), QImage::Format_Grayscale8);
    ui->labelImgHSV->setPixmap(QPixmap::fromImage(img));
}
