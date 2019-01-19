#include "coco_instance_parser.hpp"

#include "rapidjson_utility.hpp"

#include <QDebug>
#include <QDir>
#include <QImageReader>

#include <set>

namespace{

coco_instance_parser::parse_result::block::instance_info
parse_instance_info(rapidjson::Document::ConstObject const &input)
{
    coco_instance_parser::parse_result::block::instance_info box;
    auto const bbox_arr = input["bbox"].GetArray();
    box.top_left_.setX(bbox_arr[0].GetDouble());
    box.top_left_.setY(bbox_arr[1].GetDouble());
    box.bottom_right_.setX(bbox_arr[0].GetDouble() + bbox_arr[2].GetDouble());
    box.bottom_right_.setY(bbox_arr[1].GetDouble() + bbox_arr[3].GetDouble());
    box.category_id_ = input["category_id"].GetInt();

    return box;
}

}

coco_instance_parser::parse_result coco_instance_parser::parse(QString const &file_location,
                                                               QString const &image_location)
{
    parse_result result;

    auto const doc = rapid_json::json_file_to_doc(file_location.toStdString());
    auto const &anno_value = doc["annotations"];
    std::set<int> category_ids_;
    for(auto const &value : anno_value.GetArray()){
        auto const obj = value.GetObject();
        QString const image_id = QString("%1").arg(obj["image_id"].GetInt(), 12, 10, QChar('0'));
        qDebug()<<__func__<<"processing image id:"<<image_id;
        auto it = result.blocks_.find(image_id);
        auto const inst_info = parse_instance_info(obj);
        category_ids_.insert(inst_info.category_id_);
        if(it != std::end(result.blocks_)){
            it->second.instances_.emplace_back(inst_info);
        }else{
            parse_result::block blk;
            QImageReader img_reader(image_location + "/" + image_id + ".jpg");
            if(img_reader.canRead()){
                blk.height_ = img_reader.size().height();
                blk.width_ = img_reader.size().width();
            }else{
                throw std::runtime_error("cannot open image:" + img_reader.fileName().toStdString());
            }
            blk.instances_.emplace_back(inst_info);
            result.blocks_.insert(std::make_pair(image_id, blk));
        }
    }

    return result;
}
