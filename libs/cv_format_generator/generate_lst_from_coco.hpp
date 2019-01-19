#ifndef GENERATE_LST_FROM_COCO_HPP
#define GENERATE_LST_FROM_COCO_HPP

#include <QString>

/**
 * Fields need to specify in the json
 * @param folder_of_coco_image Folder contain image of coco
 * @param coco_category_want_to_detect name of the category want to detect(ex : ["person", "bicycle", "car"])
 * @param folder_of_label_image Folder save the xml files of LabelImg
 * @param save_lst_as save lst file as
 * @param save_lst_label_to_string_as save the file contain the label of lst as
 * @warning This class will generate xml files in folder_of_label before it can generate the lst file
 */
class generate_lst_from_coco
{
public:
    explicit generate_lst_from_coco(QString const &file_location);

    void apply();

private:
    QString file_location_;
};

#endif // GENERATE_LST_FROM_COCO_HPP
