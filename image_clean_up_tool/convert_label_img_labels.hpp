#ifndef CONVERT_LABEL_IMG_LABELS_HPP
#define CONVERT_LABEL_IMG_LABELS_HPP

#include <QWidget>

#include <vector>

namespace Ui {
class convert_label_img_labels;
}

class convert_label_img_labels : public QWidget
{
    Q_OBJECT

public:
    explicit convert_label_img_labels(QWidget *parent = nullptr);
    ~convert_label_img_labels();

private slots:
    void on_pushButtonAdd_clicked();

    void on_pushButtonRemove_clicked();

    void on_pushButtonConvert_clicked();

private:
    Ui::convert_label_img_labels *ui;

    std::vector<QString> file_paths_;
};

#endif // CONVERT_LABEL_IMG_LABELS_HPP
