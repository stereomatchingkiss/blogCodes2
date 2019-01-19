#include "generate_label_image_from_coco.hpp"

#include "label_image_generator.hpp"

#include "../cv_format_parser/coco_instance_parser.hpp"
#include "../cv_format_parser/json_utility.hpp"

#include <QDebug>
#include <QDir>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

#include <set>

generate_label_image_from_coco::generate_label_image_from_coco(const QString &json_location) :
    json_location_(json_location)
{
    id_to_category_.emplace_back("person");
    id_to_category_.emplace_back("bicycle");
    id_to_category_.emplace_back("car");
    id_to_category_.emplace_back("motorcycle");
    id_to_category_.emplace_back("airplane");
    id_to_category_.emplace_back("bus");
    id_to_category_.emplace_back("train");
    id_to_category_.emplace_back("truck");
    id_to_category_.emplace_back("boat");
    id_to_category_.emplace_back("traffic light");

    id_to_category_.emplace_back("fire hydrant");
    id_to_category_.emplace_back("street sign");
    id_to_category_.emplace_back("stop sign");
    id_to_category_.emplace_back("parking meter");
    id_to_category_.emplace_back("bench");
    id_to_category_.emplace_back("bird");
    id_to_category_.emplace_back("cat");
    id_to_category_.emplace_back("dog");
    id_to_category_.emplace_back("horse");
    id_to_category_.emplace_back("sheep");

    id_to_category_.emplace_back("cow");
    id_to_category_.emplace_back("elephant");
    id_to_category_.emplace_back("bear");
    id_to_category_.emplace_back("zebra");
    id_to_category_.emplace_back("giraffe");
    id_to_category_.emplace_back("hat");
    id_to_category_.emplace_back("backpack");
    id_to_category_.emplace_back("umbrella");
    id_to_category_.emplace_back("shoe");
    id_to_category_.emplace_back("eye glasses");

    id_to_category_.emplace_back("handbag");
    id_to_category_.emplace_back("tie");
    id_to_category_.emplace_back("suitcase");
    id_to_category_.emplace_back("frisbee");
    id_to_category_.emplace_back("skis");
    id_to_category_.emplace_back("snowboard");
    id_to_category_.emplace_back("sports ball");
    id_to_category_.emplace_back("kite");
    id_to_category_.emplace_back("baseball bat");
    id_to_category_.emplace_back("baseball glove");

    id_to_category_.emplace_back("skateboard");
    id_to_category_.emplace_back("surfboard");
    id_to_category_.emplace_back("tennis racket");
    id_to_category_.emplace_back("bottle");
    id_to_category_.emplace_back("plate");
    id_to_category_.emplace_back("wine glass");
    id_to_category_.emplace_back("cup");
    id_to_category_.emplace_back("fork");
    id_to_category_.emplace_back("knife");
    id_to_category_.emplace_back("spoon");

    id_to_category_.emplace_back("bowl");
    id_to_category_.emplace_back("banana");
    id_to_category_.emplace_back("apple");
    id_to_category_.emplace_back("sandwich");
    id_to_category_.emplace_back("orange");
    id_to_category_.emplace_back("broccoli");
    id_to_category_.emplace_back("carrot");
    id_to_category_.emplace_back("hot dog");
    id_to_category_.emplace_back("pizza");
    id_to_category_.emplace_back("donut");

    id_to_category_.emplace_back("cake");
    id_to_category_.emplace_back("chair");
    id_to_category_.emplace_back("couch");
    id_to_category_.emplace_back("potted plant");
    id_to_category_.emplace_back("bed");
    id_to_category_.emplace_back("mirror");
    id_to_category_.emplace_back("dining table");
    id_to_category_.emplace_back("window");
    id_to_category_.emplace_back("desk");
    id_to_category_.emplace_back("toilet");

    id_to_category_.emplace_back("door");
    id_to_category_.emplace_back("tv");
    id_to_category_.emplace_back("laptop");
    id_to_category_.emplace_back("mouse");
    id_to_category_.emplace_back("remote");
    id_to_category_.emplace_back("keyboard");
    id_to_category_.emplace_back("cell phone");
    id_to_category_.emplace_back("microwave");
    id_to_category_.emplace_back("oven");
    id_to_category_.emplace_back("toaster");

    id_to_category_.emplace_back("sink");
    id_to_category_.emplace_back("refrigerator");
    id_to_category_.emplace_back("blender");
    id_to_category_.emplace_back("book");
    id_to_category_.emplace_back("clock");
    id_to_category_.emplace_back("vase");
    id_to_category_.emplace_back("scissors");
    id_to_category_.emplace_back("teddy bear");
    id_to_category_.emplace_back("hair drier");
    id_to_category_.emplace_back("toothbrush");

    for(size_t i = 0; i != id_to_category_.size(); ++i){
        category_to_id_.emplace(id_to_category_[i], static_cast<int>(i + 1));
    }
}

void generate_label_image_from_coco::apply()
{
    auto const json_obj = json_file_to_doc(json_location_).object();
    coco_instance_parser parser;
    auto const folder_of_coco_image = json_obj["folder_of_coco_image"].toString();
    auto const parse_result = parser.parse(json_obj["annotaion_of_coco"].toString(), folder_of_coco_image);
    auto const save_xml_to = json_obj["save_label_image_xml_to"].toString();
    QDir().mkpath(save_xml_to);
    create_category_to_detect(json_obj);
    for(auto const &vpair : parse_result.blocks_){
        label_image_generator::data_to_generate data_to_gen;
        auto const obj = vpair.second;
        data_to_gen.height_ = obj.height_;
        data_to_gen.width_ = obj.width_;
        data_to_gen.abs_path_ = folder_of_coco_image + "/" + vpair.first + ".jpg";
        for(auto const &inst : obj.instances_){
            if(is_category_want_to_detect(inst.category_id_)){
                label_image_generator::data_to_generate::object lobj;
                lobj.name_ = id_to_category_[static_cast<size_t>(inst.category_id_ - 1)];
                lobj.bottom_right_ = inst.bottom_right_;
                lobj.top_left_ = inst.top_left_;
                data_to_gen.objects_.emplace_back(std::move(lobj));
            }
        }
        if(!data_to_gen.objects_.empty()){
            label_image_generator().write_data_to_file(data_to_gen, save_xml_to + "/" + vpair.first + ".xml");
        }
    }
}

void generate_label_image_from_coco::create_category_to_detect(const QJsonObject &input)
{
    auto const arr = input["coco_category_want_to_detect"].toArray();
    for(auto const &obj : arr){
        auto it = category_to_id_.find(obj.toString());
        if(it != std::end(category_to_id_)){
            qDebug()<<__func__<<"want to detect:"<<it->second<<", id:"<<obj.toString();
            category_to_detect_.insert(it->second);
        }
    }
}

bool generate_label_image_from_coco::is_category_want_to_detect(int id) const
{
    return category_to_detect_.find(id) != std::end(category_to_detect_);
}
