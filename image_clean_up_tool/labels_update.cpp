#include "labels_update.hpp"

#include <QDebug>
#include <QDomDocument>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>

labels_update::labels_update(const QString &file) :
    file_(file)
{

}

void labels_update::update_path(const QString &path)
{
    QFile file(QFileInfo(file_).absoluteFilePath());
    if(file.open(QIODevice::ReadWrite)){
        QDomDocument doc;
        doc.setContent(file.readAll());
        auto annotation = doc.firstChild().toElement();
        auto dom_elem = annotation.firstChild().toElement();
        while(!dom_elem.isNull()){
            if(dom_elem.tagName() == "path"){
                QDomElement new_node_tag = doc.createElement("path");
                QDomText new_node_text = doc.createTextNode(path);
                new_node_tag.appendChild(new_node_text);
                annotation.replaceChild(new_node_tag, dom_elem);

                file.resize(0);
                QTextStream stream;
                stream.setDevice(&file);
                doc.save(stream, 4);

                break;
            }
            dom_elem = dom_elem.nextSibling().toElement();
        }
    }else{
        qDebug()<<__func__<<"cannot open file = "<<file.fileName();
    }

}
