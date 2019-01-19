#include "generate_lst_from_coco.hpp"

#include "generate_label_image_from_coco.hpp"
#include "generate_lst_from_label_image.hpp"

generate_lst_from_coco::generate_lst_from_coco(const QString &file_location) :
    file_location_(file_location)
{

}

void generate_lst_from_coco::apply()
{
    generate_label_image_from_coco(file_location_).apply();
    generate_lst_from_label_image(file_location_).apply();
}
