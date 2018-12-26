#include "kaggle_face_detection_parser.hpp"

#include "json_utility.hpp"

#include <QDebug>

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonValue>

kaggle_face_detection_parser::parse_result kaggle_face_detection_parser::parse(const QString &file_location) const
{
    auto const docs = json_file_to_docs(file_location);
    parse_result result;
    for(auto const &doc : docs){
        if(!doc.isEmpty()){
            parse_result::block block;
            block.content_ = doc["content"].toString();
            auto const annotations = doc["annotation"].toArray();
            for(auto const &annotation : annotations){
                parse_result::block::annotation anno;
                auto const points_arr = annotation["points"].toArray();
                if(points_arr.size() == 2){
                    anno.top_left_.setX(points_arr[0]["x"].toDouble());
                    anno.top_left_.setY(points_arr[0]["y"].toDouble());
                    anno.bottom_right_.setX(points_arr[1]["x"].toDouble());
                    anno.bottom_right_.setY(points_arr[1]["y"].toDouble());
                }
                anno.height_ = annotation["imageHeight"].toInt();
                anno.width_ = annotation["imageWidth"].toInt();
                block.annos_.emplace_back(std::move(anno));
            }
            result.blocks_.emplace_back(std::move(block));
        }
    }

    return result;
}
