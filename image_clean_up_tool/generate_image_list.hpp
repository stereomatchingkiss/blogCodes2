#ifndef GENERATE_IMAGE_LIST_HPP
#define GENERATE_IMAGE_LIST_HPP

#include <QWidget>

namespace Ui {
class generate_image_list;
}

class generate_image_list : public QWidget
{
    Q_OBJECT

public:
    explicit generate_image_list(QWidget *parent = nullptr);
    ~generate_image_list();

private slots:
    void on_pushButtonAddDir_clicked();

    void on_pushButtonRemoveDir_clicked();

    void on_pushButtonGenerate_clicked();

private:
    Ui::generate_image_list *ui;
};

#endif // GENERATE_IMAGE_LIST_HPP
