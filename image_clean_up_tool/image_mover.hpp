#ifndef IMAGE_MOVER_HPP
#define IMAGE_MOVER_HPP

#include <vector>

#include <QWidget>

namespace Ui {
class image_mover;
}

class QLineEdit;

class scroll_area_key_press_eater : public QObject
{
    Q_OBJECT
public:
    explicit scroll_area_key_press_eater(QObject *parent);

signals:
    void key_press(QKeyEvent *event);

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
};

class image_mover : public QWidget
{
    Q_OBJECT
public:
    explicit image_mover(QWidget *parent = nullptr);
    ~image_mover() override;

private slots:    
    void on_pushButtonSelectImageFolder_clicked();

    void on_pushButtonLoadImages_clicked();

    void on_pushButtonPrev_clicked();

    void on_pushButtonNext_clicked();    

    void on_spinBoxIndex_valueChanged(int arg1);

    void on_pushButtonRestore_clicked();

    void on_pushButtonAddNewKey_clicked();

    void on_pushButtonDeleteKey_clicked();

    void on_pushButtonOpenImage_clicked();

private:
    void keyPressEvent(QKeyEvent *event) override;

    void load_images(size_t image_index);
    void move_file(QString const &target_dir);
    void select_folder(QLineEdit *editor);
    void set_number(QSize const &img_size);
    void show_image();

    Ui::image_mover *ui;

    size_t image_index_ = 0;
    std::vector<QString> images_urls_;
    std::vector<std::tuple<QString, QString>> steps_record_;
};

#endif // IMAGE_MOVER_HPP
