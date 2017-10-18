#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>

#include <opencv2/core.hpp>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButtonOpen_clicked();

    void on_horizontalSliderHueLow_valueChanged(int value);

    void on_horizontalSliderSatLow_valueChanged(int value);

    void on_horizontalSliderValLow_valueChanged(int value);

    void on_horizontalSliderHueHigh_valueChanged(int value);

    void on_horizontalSliderSatHigh_valueChanged(int value);

    void on_horizontalSliderValHigh_valueChanged(int value);

private:
    void update_hsv_img();

    Ui::MainWindow *ui;

    cv::Mat input_;
    cv::Mat hsv_;
};

#endif // MAINWINDOW_HPP
