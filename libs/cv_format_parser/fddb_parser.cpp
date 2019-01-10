#include "fddb_parser.hpp"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfoList>
#include <QImageReader>
#include <QTextStream>

fddb_parser::parse_result::block fddb_parser::parse_block(int &i,
                                                          QStringList const &contents,
                                                          QString const &folder_location) const
{
    parse_result::block blk;
    blk.relative_img_path_ = contents[i++] + ".jpg";
    blk.abs_img_path_ = folder_location + "/" + blk.relative_img_path_;
    qDebug()<<"processing image:"<<blk.abs_img_path_;
    QImageReader const img_reader(blk.abs_img_path_);
    if(img_reader.canRead()){
        blk.height_ = img_reader.size().height();
        blk.width_ = img_reader.size().width();
        int const num_of_face = contents[i++].toInt();
        for(int j = 0; j != num_of_face; ++j){
            parse_result::block::annotation anno;
            auto const face_anno = contents[i++].split(" ");
            if(face_anno.size() >= 5){
                auto const x_radius = face_anno[0].toDouble();
                auto const y_radius = face_anno[1].toDouble();
                auto const x_center = face_anno[3].toDouble();
                auto const y_center = face_anno[4].toDouble();
                anno.top_left_.setX(std::max(0.0, x_center - x_radius));
                anno.top_left_.setY(std::max(0.0, y_center - y_radius));
                anno.bottom_right_.setX(std::min(blk.width_ - 1.0, x_center + x_radius));
                anno.bottom_right_.setY(std::min(blk.height_ - 1.0, y_center + y_radius));
                blk.annos_.emplace_back(std::move(anno));
            }else{
                qDebug()<<"Annotaion of face less than 5";
            }
        }
    }else{
        qDebug()<<QString("image %1 cannot read").arg(blk.abs_img_path_);
    }

    return blk;
}

fddb_parser::parse_result fddb_parser::parse(const QString &folder_location) const
{
    auto const info_list = QDir(folder_location + "/FDDB-folds").entryInfoList(QStringList()<<"*txt",
                                                                               QDir::Files | QDir::NoDotAndDotDot);
    if(!info_list.isEmpty()){
        qDebug()<<"size of files:"<<info_list.size();
        parse_result result;
        for(auto const &finfo : info_list){
            if(finfo.fileName().contains("ellipseList")){
                qDebug()<<"processing file:"<<finfo.fileName();
                QFile file(finfo.absoluteFilePath());
                if(file.open(QIODevice::ReadOnly)){
                    QTextStream stream(&file);
                    auto contents = stream.readAll().split("\n");
                    if(contents.back().isEmpty()){
                        contents.pop_back();
                    }
                    qDebug()<<contents.size();

                    for(int i = 0; i < contents.size();){
                        auto blk = parse_block(i, contents, folder_location);
                        if(!blk.annos_.empty()){
                            result.blocks_.emplace_back(std::move(blk));
                        }else{
                            qDebug()<<"parse procedure error";
                        }
                    }
                }else{
                    qDebug()<<"cannot open file";
                }
            }
        }

        return result;
    }

    return {};
}
