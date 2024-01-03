#ifndef WIDGET_EXPORT_OPEN_IMAGE_VIEWER_TO_OTHER_FORMAT_HPP
#define WIDGET_EXPORT_OPEN_IMAGE_VIEWER_TO_OTHER_FORMAT_HPP

#include "../structure/open_image_detector_info.hpp"

#include <QWidget>

#include <map>
#include <set>

namespace Ui {
class widget_export_open_image_viewer_to_other_format;
}

class widget_export_open_image_viewer_to_other_format : public QWidget
{
    Q_OBJECT

public:
    explicit widget_export_open_image_viewer_to_other_format(QWidget *parent = nullptr);
    ~widget_export_open_image_viewer_to_other_format();

    void set_detector_info_list(std::map<QString, std::vector<open_image_detector_info>> *input);
    void set_key_to_labels(std::map<QString, QString> *key_to_labels);
    void update_table(QStringList const &input);

private slots:
    void on_pushButtonExport_clicked();

private:
    Ui::widget_export_open_image_viewer_to_other_format *ui;

    std::map<QString, std::vector<open_image_detector_info>> *detector_info_list_ = nullptr;
    std::map<QString, QString> *key_to_labels_ = nullptr;
};

#endif // WIDGET_EXPORT_OPEN_IMAGE_VIEWER_TO_OTHER_FORMAT_HPP
