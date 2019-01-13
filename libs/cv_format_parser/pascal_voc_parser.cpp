#include "pascal_voc_parser.hpp"

#include <QDebug>
#include <QDomDocument>
#include <QFile>
#include <QStringList>

pascal_voc_parser::parse_result pascal_voc_parser::parse(const QString &file_location) const
{
    parse_result result;
    QDomDocument doc;
    QFile file(file_location);
    if(!file.open(QIODevice::ReadOnly) || !doc.setContent(&file)){
        qDebug()<<__func__<<"cannot open file:"<<file_location;
        return {};
    }

    QDomElement component= doc.documentElement().firstChild().toElement();
    while(!component.isNull()){
        if(component.tagName() == "filename"){
            result.file_name_ = component.firstChild().toText().data();
        }
        if(component.tagName() == "folder"){
            result.folder_ = component.firstChild().toText().data();
        }

        parse_width_height(result, component);
        parse_object(result, component);

        component = component.nextSibling().toElement();
    }

    return result;
}

void pascal_voc_parser::parse_action(pascal_voc_parser::parse_result::object &inout, const QDomElement &component) const
{
    if(component.tagName() == "actions"){
        auto child = component.firstChild().toElement();
        while(!child.isNull()){
            if(child.firstChild().toText().data().toInt() == 1){
                inout.act_ = child.tagName();
            }
            child = child.nextSibling().toElement();
        }
    }
}

void pascal_voc_parser::parse_bndbox(pascal_voc_parser::parse_result::object &inout, const QDomElement &component) const
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

void pascal_voc_parser::parse_object(pascal_voc_parser::parse_result &inout, const QDomElement &component) const
{
    if(component.tagName() == "object"){
        parse_result::object obj;
        auto child = component.firstChild().toElement();
        while(!child.isNull()){
            if(child.tagName() == "name"){
                obj.name_ = child.firstChild().toText().data();
            }
            parse_action(obj, child);
            parse_bndbox(obj, child);
            child = child.nextSibling().toElement();
        }
        inout.objects_.emplace_back(std::move(obj));
    }
}

void pascal_voc_parser::parse_width_height(pascal_voc_parser::parse_result &inout, const QDomElement &component) const
{
    if(component.tagName() == "size"){
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
