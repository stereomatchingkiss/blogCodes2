#ifndef REMOVE_DUPLICATE_IMAGES_HPP
#define REMOVE_DUPLICATE_IMAGES_HPP

#include <QWidget>

#include <vector>

namespace Ui {
class remove_duplicate_images;
}

class remove_duplicate_images : public QWidget
{
    Q_OBJECT

public:
    explicit remove_duplicate_images(QWidget *parent = nullptr);
    ~remove_duplicate_images();

private slots:
    void on_pushButtonSelectFolder_clicked();

    void on_pushButtonRemove_clicked();

private:
    void load_image_urls();

    Ui::remove_duplicate_images *ui;

    std::vector<QString> image_urls_;
};

#endif // REMOVE_DUPLICATE_IMAGES_HPP
