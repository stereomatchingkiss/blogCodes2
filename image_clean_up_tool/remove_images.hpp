#ifndef REMOVE_IMAGES_HPP
#define REMOVE_IMAGES_HPP

#include <QWidget>

#include <deque>
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
    void on_pushButtonRemove_clicked();

    void on_pushButtonAddFolder_clicked();

    void on_pushButtonFolder_clicked();

private:
    bool get_checkbox_state(int row, int column) const;
    void load_image_urls();
    size_t remove_duplicate_images();
    size_t remove_img_size_less_than();

    Ui::remove_images *ui;

    std::vector<QString> image_urls_;
    std::deque<bool> image_urls_can_remove_;
};

#endif // REMOVE_IMAGES_HPP
