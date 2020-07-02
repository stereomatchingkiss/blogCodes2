#ifndef IMAGE_MOVER_HPP
#define IMAGE_MOVER_HPP

#include <vector>

#include <QWidget>

namespace Ui {
class image_mover;
}

class QLineEdit;

class image_mover : public QWidget
{
    Q_OBJECT
public:
    explicit image_mover(QWidget *parent = nullptr);
    ~image_mover() override;

private slots:
    void on_pushButtonSelectFolder_1_clicked();

    void on_pushButtonSelectFolder_2_clicked();

    void on_pushButtonSelectFolder_3_clicked();

    void on_pushButtonSelectFolder_4_clicked();

    void on_pushButtonSelectFolder_5_clicked();

    void on_pushButtonSelectImageFolder_clicked();

    void on_pushButtonLoadImages_clicked();

    void on_pushButtonPrev_clicked();

    void on_pushButtonNext_clicked();

private:
    void keyPressEvent(QKeyEvent *event) override;

    void move_file(QString const &target_dir);
    void select_folder(QLineEdit *editor);
    void set_number();

    Ui::image_mover *ui;

    size_t image_index_ = 0;
    std::vector<QString> images_urls_;
    void show_image();
};

#endif // IMAGE_MOVER_HPP
