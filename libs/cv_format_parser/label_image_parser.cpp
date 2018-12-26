#include "label_image_parser.hpp"

#include <QDebug>
#include <QDomDocument>
#include <QFile>
#include <QStringList>

label_image_parser::parse_result label_image_parser::parse(const QString &file_location) const
{
    parse_result result;
    QDomDocument doc;
    QFile file(file_location);
    if(!file.open(QIODevice::ReadOnly) || !doc.setContent(&file)){
        qDebug()<<__func__<<"cannot open file:"<<file_location;
        return {};
    }

    qDebug()<<"can open file";
    QDomElement component= doc.documentElement().firstChild().toElement();
    while(!component.isNull()){
        if(component.tagName() == "path"){
            result.abs_path_ = component.firstChild().toText().data();
        }
        parse_width_height(result, component);
        parse_object(result, component);


        component = component.nextSibling().toElement();
    }

    return result;
}

std::vector<label_image_parser::parse_result> label_image_parser::parse(const QStringList &files_location) const
{
    std::vector<parse_result> results;
    for(auto const &file_location : files_location){
        results.emplace_back(parse(file_location));
    }

    return results;
}

void label_image_parser::parse_bndbox(parse_result::object &inout, const QDomElement &component) const
{
    if(component.tagName() == "bndbox"){
        auto child = component.firstChild().toElement();
        while(!child.isNull()){
            if(child.tagName() == "xmin"){
                inout.top_left_.setX(child.firstChild().toText().data().toInt());
            }
            if(child.tagName() == "ymin"){
                inout.top_left_.setY(child.firstChild().toText().data().toInt());
            }
            if(child.tagName() == "xmax"){
                inout.bottom_right_.setX(child.firstChild().toText().data().toInt());
            }
            if(child.tagName() == "ymax"){
                inout.bottom_right_.setY(child.firstChild().toText().data().toInt());
            }
            child = child.nextSibling().toElement();
        }
    }
}

void label_image_parser::parse_object(label_image_parser::parse_result &inout, const QDomElement &component) const
{
    if(component.tagName() == "object"){
        parse_result::object obj;
        auto child = component.firstChild().toElement();
        while(!child.isNull()){
            if(child.tagName() == "name"){
                obj.name_ = child.firstChild().toText().data();
            }
            parse_bndbox(obj, child);
            child = child.nextSibling().toElement();
        }
        inout.objects_.emplace_back(std::move(obj));
    }
}

void label_image_parser::parse_width_height(label_image_parser::parse_result &inout, const QDomElement &component) const
{
    if(component.tagName() == "size"){
        qDebug()<<"found size";
        auto child = component.firstChild().toElement();
        while(!child.isNull()){
            if(child.tagName() == "height"){
                inout.height_ = child.firstChild().toText().data().toInt();
            }
            if(child.tagName() == "width"){
                inout.width_ = child.firstChild().toText().data().toInt();
            }
            child = child.nextSibling().toElement();
        }
    }
}
