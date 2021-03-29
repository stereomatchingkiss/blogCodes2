#ifndef MASK_ADJUSTMENT_WIDGET_HPP
#define MASK_ADJUSTMENT_WIDGET_HPP

#include <QWidget>

#include <opencv2/core.hpp>

namespace Ui {
class mask_adjustment_widget;
}

class mask_adjustment_widget : public QWidget
{
    Q_OBJECT

public:
    explicit mask_adjustment_widget(QWidget *parent = nullptr);
    ~mask_adjustment_widget() override;

private slots:
    void on_pushButtonLoadImages_clicked();

    void on_pushButtonReloadImg_clicked();

    void on_pushButtonToZero_clicked();

    void on_pushButtonPrev_clicked();

    void on_pushButtonNext_clicked();

    void on_pushButtonSave_clicked();

    void on_spinBoxToImage_valueChanged(int arg1);

    void on_pushButtonAdjustMaskByAlgo_clicked();

private:
    void load_images(size_t image_index);
    void reload_image();
    void set_number();
    void show_image();
    void keyPressEvent(QKeyEvent *event) override;

    Ui::mask_adjustment_widget *ui;

    size_t image_index_ = 0;
    std::vector<QString> images_urls_img_;
    std::vector<QString> images_urls_mask_;
    cv::Mat cimg_origin_;
    cv::Mat cmask_;
    cv::Mat cscene_;
    cv::Mat cscene_origin_;
};

#endif // MASK_ADJUSTMENT_WIDGET_HPP
