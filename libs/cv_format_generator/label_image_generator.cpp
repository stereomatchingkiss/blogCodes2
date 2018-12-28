#include "label_image_generator.hpp"

#include <QDebug>
#include <QDomDocument>
#include <QFile>
#include <QFileInfo>
#include <QStringList>
#include <QTextStream>

void label_image_generator::append_source()
{
    auto source_elem = doc_.createElement("source");
    source_elem.appendChild(create_child("database", "Unknown"));
    root_.appendChild(source_elem);
}

void label_image_generator::append_size()
{
    auto size_elem = doc_.createElement("size");
    size_elem.appendChild(create_child("width", QString::number(data_to_gen_.width_)));
    size_elem.appendChild(create_child("height", QString::number(data_to_gen_.height_)));
    size_elem.appendChild(create_child("depth", QString::number(3)));
    root_.appendChild(size_elem);
}

void label_image_generator::append_objects()
{
    for(auto const &obj : data_to_gen_.objects_){
        auto obj_elem = doc_.createElement("object");
        obj_elem.appendChild(create_child("name", obj.name_));
        obj_elem.appendChild(create_child("pose", "Unspecified"));
        obj_elem.appendChild(create_child("truncated", QString::number(0)));
        obj_elem.appendChild(create_child("difficult", QString::number(0)));

        auto bbox_elem = doc_.createElement("bndbox");
        bbox_elem.appendChild(create_child("xmin", QString::number(static_cast<int>(obj.top_left_.x()))));
        bbox_elem.appendChild(create_child("ymin", QString::number(static_cast<int>(obj.top_left_.y()))));
        bbox_elem.appendChild(create_child("xmax", QString::number(static_cast<int>(obj.bottom_right_.x()))));
        bbox_elem.appendChild(create_child("ymax", QString::number(static_cast<int>(obj.bottom_right_.y()))));

        obj_elem.appendChild(bbox_elem);

        root_.appendChild(obj_elem);
    }
}

void label_image_generator::append_path()
{
    data_to_gen_.abs_path_ = data_to_gen_.abs_path_.replace("\\", "/");
    auto const paths = data_to_gen_.abs_path_.split("/");
    if(paths.size() >= 2){
        root_.appendChild(create_child("folder", paths[paths.size() - 2]));
    }
}

void label_image_generator::write_data_to_file(data_to_generate const &input, QString const &file_location)
{
    QFile file(file_location);
    if(!file.open(QIODevice::WriteOnly)){
        qDebug()<<__func__<<"cannot write data into the file:"<<file_location;
    }

    data_to_gen_ = input;
    doc_.clear();
    root_ = doc_.createElement("annotation");
    doc_.appendChild(root_);

    append_path();
    root_.appendChild(create_child("filename", QFileInfo(data_to_gen_.abs_path_).fileName()));
    root_.appendChild(create_child("path", data_to_gen_.abs_path_));
    append_source();
    append_size();
    root_.appendChild(create_child("segmented", QString::number(0)));
    append_objects();

    QTextStream stream(&file);
    stream<<doc_.toString();

    doc_.clear();
}

QDomElement label_image_generator::create_child(const QString &tag_name, const QString &node_value)
{
    auto elem = doc_.createElement(tag_name);
    elem.appendChild(doc_.createTextNode(node_value));

    return elem;
}
