#ifndef IMAGE_CROPPER_HPP
#define IMAGE_CROPPER_HPP

#include <QWidget>

namespace Ui {
class image_cropper;
}

class image_cropper : public QWidget
{
    Q_OBJECT

public:
    explicit image_cropper(QWidget *parent = nullptr);
    ~image_cropper();

private slots:
    void on_pushButtonSelectImage_clicked();

    void on_pushButtonSplit_clicked();

    void on_pushButtonSeectFolder_clicked();

    void on_pushButtonSplitImages_clicked();

private:
    bool split_image(QString const &img_url) const;

    Ui::image_cropper *ui;
};

#endif // IMAGE_CROPPER_HPP
