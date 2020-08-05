#ifndef CHANGE_WRONG_IMAGE_SUFFIX_HPP
#define CHANGE_WRONG_IMAGE_SUFFIX_HPP

#include <QWidget>

namespace Ui {
class change_wrong_image_suffix;
}

class change_wrong_image_suffix : public QWidget
{
    Q_OBJECT

public:
    explicit change_wrong_image_suffix(QWidget *parent = nullptr);
    ~change_wrong_image_suffix();

private slots:
    void on_pushButtonConvert_clicked();

private:
    Ui::change_wrong_image_suffix *ui;
};

#endif // CHANGE_WRONG_IMAGE_SUFFIX_HPP
