#ifndef GENERATE_LST_FROM_LABEL_IMAGE_HPP
#define GENERATE_LST_FROM_LABEL_IMAGE_HPP

#include <QFileInfoList>
#include <QJsonObject>
#include <QString>

#include <map>

class generate_lst_from_label_image
{
public:
    explicit generate_lst_from_label_image(QString const &json_location);

    void apply();

private:
    void print_name_to_label_index(std::map<QString, size_t> const &input);

    std::map<QString, size_t> category_to_id_;
    QJsonObject json_obj_;
    QString save_as_;
    QFileInfoList xml_info_;
};

#endif // GENERATE_LST_FROM_LABEL_IMAGE_HPP
