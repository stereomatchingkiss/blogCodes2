#ifndef CONVERT_YOLO_DETECT_RESULTS_TO_LABEL_IMG_ANNOTATIONS_HPP
#define CONVERT_YOLO_DETECT_RESULTS_TO_LABEL_IMG_ANNOTATIONS_HPP

#include <QWidget>

namespace Ui {
class convert_yolo_detect_results;
}

class convert_yolo_detect_results : public QWidget
{
    Q_OBJECT

public:
    explicit convert_yolo_detect_results(QWidget *parent = nullptr);
    ~convert_yolo_detect_results();

private slots:
    void on_pushButtonConvert_clicked();

private:
    Ui::convert_yolo_detect_results *ui;
};

#endif // CONVERT_YOLO_DETECT_RESULTS_TO_LABEL_IMG_ANNOTATIONS_HPP
