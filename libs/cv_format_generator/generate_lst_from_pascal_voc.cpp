#include "generate_lst_from_pascal_voc.hpp"

#include "../cv_format_parser/json_utility.hpp"
#include "../cv_format_parser/pascal_voc_parser.hpp"

#include "mxnet_lst_generator.hpp"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QTextStream>

namespace{

QPointF normalize(QPointF const &input, int width, int height)
{
    QPointF result;
    result.setX(input.x() / width);
    result.setY(input.y() / height);

    return result;
}

}

generate_lst_from_pascal_voc::generate_lst_from_pascal_voc(const QString &json_location) :
    json_obj_(json_file_to_doc(json_location).object())
{

}

void generate_lst_from_pascal_voc::apply()
{
    pascal_voc_parser parser;
    mxnet_lst_generator lst_gen(json_obj_["save_lst_as"].toString());
    qDebug()<<json_obj_["folder_of_pascal"].toString() + "/Annotations";
    auto const info_list = QDir(json_obj_["folder_of_pascal"].toString() + "/Annotations").
            entryInfoList(QDir::NoDotAndDotDot | QDir::Files);
    auto const val_set = get_person_for_validate();

    for(auto const &info : info_list){
        auto const result = parser.parse(info.absoluteFilePath());
        if(std::end(val_set) != val_set.find(QFileInfo(result.file_name_).baseName())){
            mxnet_lst_generator::data_to_generate dtg;
            dtg.height_ = result.height_;
            dtg.width_ = result.width_;
            dtg.img_path_ = result.file_name_;
            for(auto const &anno : result.objects_){
                if(anno.name_ == "person"){
                    mxnet_lst_generator::data_to_generate::label lb;
                    lb.bottom_right_.setX(std::min(result.width_ - 1.0, anno.bottom_right_.x()));
                    lb.bottom_right_.setY(std::min(result.height_ - 1.0, anno.bottom_right_.y()));
                    lb.top_left_.setX(std::max(0.0, anno.top_left_.x()));
                    lb.top_left_.setY(std::max(0.0, anno.top_left_.y()));
                    lb.bottom_right_ = normalize(anno.bottom_right_, dtg.width_, dtg.height_);
                    lb.top_left_ = normalize(anno.top_left_, dtg.width_, dtg.height_);
                    lb.id_ = 1;
                    dtg.labels_.emplace_back(std::move(lb));
                }
            }
            if(!dtg.labels_.empty()){
                lst_gen.apply(dtg);
            }
        }
    }
}

std::set<QString> generate_lst_from_pascal_voc::get_person_for_validate() const
{
    QFile file(json_obj_["folder_of_pascal"].toString() + "/ImageSets/Main/person_val.txt");
    std::set<QString> result;
    QDir().mkpath("images");
    if(file.open(QIODevice::ReadOnly)){
        auto const contents = file.readAll().split('\n');
        for(auto const &content : contents){
            auto const line = QString(content).split(QChar(' '), QString::SkipEmptyParts);
            if(line.size() == 2 && line[1].toInt() == 1){
                result.insert(line[0]);
            }
        }
    }

    return result;
}
