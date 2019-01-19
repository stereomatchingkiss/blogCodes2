#ifndef GENERATE_LABEL_IMAGE_FROM_COCO_HPP
#define GENERATE_LABEL_IMAGE_FROM_COCO_HPP

#include <QString>

#include <map>
#include <set>
#include <vector>

class QJsonObject;

class generate_label_image_from_coco
{
public:
    explicit generate_label_image_from_coco(QString const &json_location);

    void apply();

private:
    void create_category_to_detect(QJsonObject const &input);
    bool is_category_want_to_detect(int id) const;

    std::set<int> category_to_detect_;
    std::map<QString, int> category_to_id_;
    std::vector<QString> id_to_category_;
    QString const json_location_;
};

#endif // GENERATE_LABEL_IMAGE_FROM_COCO_HPP
