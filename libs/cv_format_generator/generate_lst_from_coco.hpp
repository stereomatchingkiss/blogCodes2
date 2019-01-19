#ifndef GENERATE_LST_FROM_COCO_HPP
#define GENERATE_LST_FROM_COCO_HPP

#include <QString>

/**
 * Fields need to specify in the json 
 * @param annotaion_of_coco Annotaion of coco(ex : instances_train2017.json)
 * @param coco_category_want_to_detect name of the category want to detect(ex : ["person", "bicycle", "car"])
 * @param folder_of_coco_image Folder contain image of coco
 * @param folder_of_label_image_xml Folder contain the xml files of LabelImg
 * @param folder_to_save_label_img_xml Folder to save the xml files of LabelImg
 * @param lst_category_to_id Map category to id, ex : [
  {"category" : "person", "id" : 0}, {"category" : "bicycle", "id" : 1}, {"category" : "car", "id" : 2},
  {"category" : "motorcycle", "id" : 3}, {"category" : "bus", "id" : 4}, {"category" : "train", "id" : 5},
  {"category" : "truck", "id" : 6}]
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
