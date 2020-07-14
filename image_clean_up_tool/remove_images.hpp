#ifndef REMOVE_IMAGES_HPP
#define REMOVE_IMAGES_HPP

#include <QWidget>

#include <vector>

namespace Ui {
class remove_images;
}

class remove_images : public QWidget
{
    Q_OBJECT

public:
    explicit remove_images(QWidget *parent = nullptr);
    ~remove_images();

private slots:
    void on_pushButtonSelectFolder_clicked();

    void on_pushButtonRemove_clicked();

private:
    void load_image_urls();

    Ui::remove_images *ui;

    std::vector<QString> image_urls_;
};

#endif // REMOVE_IMAGES_HPP
