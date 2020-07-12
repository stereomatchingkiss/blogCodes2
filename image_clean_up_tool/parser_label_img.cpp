#include "parser_label_img.hpp"

#include <QDebug>
#include <QDomDocument>
#include <QFile>
#include <QFileInfo>
#include <QImageReader>

namespace{

QRectF parse_bndbox(QDomElement &elem)
{
    QRectF rect;
    while(!elem.isNull()){
        if(elem.tagName() == "xmin"){
            rect.setX(elem.firstChild().toText().data().toDouble());
        }else if(elem.tagName() == "ymin"){
            rect.setY(elem.firstChild().toText().data().toDouble());
        }else if(elem.tagName() == "xmax"){
            rect.setRight(elem.firstChild().toText().data().toDouble());
        }else{
            rect.setBottom(elem.firstChild().toText().data().toDouble());
        }
        elem = elem.nextSibling().toElement();
    }

    return rect;
}

parser_label_img::parse_data parse_xml(const QString &fname)
{
    QFile file(QFileInfo(fname).absoluteFilePath());
    parser_label_img::parse_data result;
    if(file.open(QIODevice::ReadOnly)){
        QDomDocument doc;
        doc.setContent(&file);
        auto dom_elem = doc.firstChild().toElement().firstChild().toElement();
        while(!dom_elem.isNull()){
            if(dom_elem.tagName() == "path"){
                result.abs_file_path_ = dom_elem.firstChild().toText().data();
            }else if(dom_elem.tagName() == "object"){
                parser_label_img::parse_data::object obj;
                auto child = dom_elem.firstChild().toElement();
                while(!child.isNull()){
                    if(child.tagName() == "name"){
                        obj.label_ = child.firstChild().toText().data();
                    }else if(child.tagName() == "bndbox"){
                        auto bndbox = child.firstChild().toElement();
                        obj.rect_ = parse_bndbox(bndbox);
                    }

                    child = child.nextSibling().toElement();
                }
                result.objects_.emplace_back(obj);
            }

            dom_elem = dom_elem.nextSibling().toElement();
        }
    }else{
        qDebug()<<__func__<<"cannot open file = "<<file.fileName();
    }
    return result;
}


}

parser_label_img::parser_label_img()
{

}

parser_label_img::parse_data parser_label_img::parse(const QString &fname) const
{
    return parse_xml(fname);
}

void parser_label_img::parse_data::print() const
{
    qDebug()<<__func__<<": file path = "<<abs_file_path_;
    for(auto const &obj : objects_){
        qDebug()<<__func__<<": label = "<<obj.label_<<", rect = "<<obj.rect_;
    }
}

