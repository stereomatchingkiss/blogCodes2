#ifndef WIDGET_OPEN_IMAGE_VIEWER_HPP
#define WIDGET_OPEN_IMAGE_VIEWER_HPP

#include <QFileInfoList>

#include <QWidget>

#include <map>
#include <set>

namespace Ui {
class widget_open_image_viewer;
}

class widget_open_image_viewer : public QWidget
{
    Q_OBJECT

public:
    explicit widget_open_image_viewer(QWidget *parent = nullptr);
    ~widget_open_image_viewer();

private slots:
    void on_pushButtonLoad_clicked();

    void on_spinBoxSelectImage_valueChanged(int arg1);

private:
    struct detection_info{
        QString label_;
        float xmin_;
        float xmax_;
        float ymin_;
        float ymax_;
    };

    void draw_detection_info(QImage &img, QString const &im_path) const;
    void load_images_path();
    void load_key_to_labels();
    detection_info strs_to_detection_info(QStringList const &input) const;

    Ui::widget_open_image_viewer *ui;

    std::map<QString, std::vector<detection_info>> detector_info_list_;
    std::set<QString> images_base_name_;
    std::map<QString, QString> key_to_labels_;
};

#endif // WIDGET_OPEN_IMAGE_VIEWER_HPP
