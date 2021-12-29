#ifndef COLOR_FILTER_WIDGET_HPP
#define COLOR_FILTER_WIDGET_HPP

#include <QWidget>
#include <QImage>

#include <opencv2/core.hpp>

namespace Ui {
class color_filter_widget;
}

class color_filter_widget : public QWidget
{
    Q_OBJECT

public:
    explicit color_filter_widget(QWidget *parent = nullptr);
    ~color_filter_widget();

private slots:
    void on_pushButtonSelectImage_clicked();

    void on_pushButtonApply_clicked();

    void on_horizontalSliderHueLow_valueChanged(int value);

    void on_horizontalSliderHueUp_valueChanged(int value);

private:
    void update_hsv_image();

    Ui::color_filter_widget *ui;

    cv::Mat origin_img_;
    QString last_dir_ = "/home";
};

#endif // COLOR_FILTER_WIDGET_HPP
