#include "generate_lst_from_label_image.hpp"

#include "../cv_format_parser/json_utility.hpp"
#include "../cv_format_parser/label_image_parser.hpp"

#include "mxnet_lst_generator.hpp"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QTextStream>

#include <map>

namespace{

QPointF normalize(QPointF const &input, int width, int height)
{
    QPointF result;
    result.setX(input.x() / width);
    result.setY(input.y() / height);

    return result;
}

}

generate_lst_from_label_image::generate_lst_from_label_image(const QString &json_location)
{
    json_obj_ = json_file_to_doc(json_location).object();
    save_as_ = json_obj_["save_lst_as"].toString();
    QDir dir(json_obj_["folder_of_label_image"].toString());
    xml_info_ = dir.entryInfoList(QStringList()<<"*.xml", QDir::NoDotAndDotDot | QDir::Files);
}

void generate_lst_from_label_image::apply()
{
    label_image_parser label_parser;
    mxnet_lst_generator lst_gen(save_as_);
    std::map<QString, size_t> label_index_map;
    size_t label_index = 0;
    for(int i = 0; i != xml_info_.size(); ++i){
        qDebug()<<"processing file:"<<xml_info_[i].fileName();
        auto const parse_result = label_parser.parse(xml_info_[i].absoluteFilePath());
        mxnet_lst_generator::data_to_generate data_to_gen;
        data_to_gen.height_ = parse_result.height_;
        data_to_gen.width_ = parse_result.width_;
        data_to_gen.index_ = static_cast<size_t>(i);
        data_to_gen.img_path_ = QFileInfo(parse_result.abs_path_).fileName();
        for(auto const &val : parse_result.objects_){
            mxnet_lst_generator::data_to_generate::label lst_label;
            lst_label.bottom_right_.setX(std::min(parse_result.width_ - 1.0, val.bottom_right_.x()));
            lst_label.bottom_right_.setY(std::min(parse_result.height_ - 1.0, val.bottom_right_.y()));
            lst_label.top_left_.setX(std::max(0.0, val.top_left_.x()));
            lst_label.top_left_.setY(std::max(0.0, val.top_left_.y()));

            lst_label.bottom_right_ = normalize(lst_label.bottom_right_, parse_result.width_, parse_result.height_);
            lst_label.top_left_ = normalize(lst_label.top_left_, parse_result.width_, parse_result.height_);
            auto it = label_index_map.find(val.name_);
            if(it != std::end(label_index_map)){
                lst_label.id_ = it->second;
            }else{
                lst_label.id_ = label_index;
                label_index_map.insert(std::make_pair(val.name_, label_index++));
            }
            data_to_gen.labels_.emplace_back(std::move(lst_label));
        }
        lst_gen.apply(data_to_gen);
    }

    print_name_to_label_index(label_index_map);
}

void generate_lst_from_label_image::print_name_to_label_index(const std::map<QString, size_t> &input)
{
    QFile file(json_obj_["save_lst_label_to_string_as"].toString());
    if(file.open(QIODevice::WriteOnly)){
        std::map<size_t, QString> result;
        for(auto const &vpair : input){
            result.insert(std::make_pair(vpair.second, vpair.first));
        }

        QTextStream stream(&file);
        for(auto const &vpair : result){
           stream<<vpair.first<<"\t"<<vpair.second<<"\n";
        }
    }else{
        qDebug()<<__func__<<"cannot write label to string to:"<<file.fileName();
    }
}
