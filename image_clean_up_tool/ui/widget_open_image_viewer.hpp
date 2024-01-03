#ifndef WIDGET_OPEN_IMAGE_VIEWER_HPP
#define WIDGET_OPEN_IMAGE_VIEWER_HPP

#include "../structure/open_image_detector_info.hpp"

#include <QFileInfoList>

#include <QWidget>

#include <map>
#include <set>

namespace Ui {
class widget_open_image_viewer;
}

class widget_export_open_image_viewer_to_other_format;

class widget_open_image_viewer : public QWidget
{
    Q_OBJECT

public:
    explicit widget_open_image_viewer(QWidget *parent = nullptr);
    ~widget_open_image_viewer();

private slots:
    void on_pushButtonLoad_clicked();

    void on_spinBoxSelectImage_valueChanged(int arg1);

    void on_pushButtonExportToOtherFormat_clicked();

private:
    void draw_detection_info(QImage &img, QString const &im_path) const;
    void load_creator_info_list();
    void load_images_path();
    void load_key_to_labels();
    open_image_detector_info strs_to_detection_info(QStringList const &input) const;

    Ui::widget_open_image_viewer *ui;

    std::map<QString, std::vector<open_image_detector_info>> detector_info_list_;
    std::set<QString> images_base_name_;
    std::map<QString, QString> key_to_labels_;
    std::set<QString> unique_labels_;

    std::unique_ptr<widget_export_open_image_viewer_to_other_format> widget_export_open_image_viewer_to_other_format_;
};

#endif // WIDGET_OPEN_IMAGE_VIEWER_HPP
