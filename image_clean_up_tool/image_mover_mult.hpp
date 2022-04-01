#ifndef IMAGE_MOVER_MULT_HPP
#define IMAGE_MOVER_MULT_HPP

#include <QWidget>

#include <QStringList>
#include <QVector>

namespace Ui {
class image_mover_mult;
}

class QTableWidget;

/**
 * @todo Finish this class
 */
class image_mover_mult : public QWidget
{
    Q_OBJECT

public:
    explicit image_mover_mult(QWidget *parent = nullptr);
    ~image_mover_mult();

private slots:
    void on_pushButtonLoadImages_clicked();

    void on_pushButtonSelectImageFolder_clicked();

    void on_pushButtonSetFolderMoveTo_clicked();

    void on_pushButtonMoveImages_clicked();

private:
    void show_page(int index);

    Ui::image_mover_mult *ui;

    QStringList images_urls_;
    QVector<int> images_url_move_target_;
    QTableWidget *move_folder_widget_;
};

#endif // IMAGE_MOVER_MULT_HPP
