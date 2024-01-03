#include "json_utils.hpp"

#include <QFile>
#include <QTextStream>

#include <QJsonDocument>
#include <QJsonObject>

namespace flt::json{

QJsonObject parse_file_to_jobj(const QString &fname)
{
    if(QFile file_obj(fname); file_obj.open(QIODevice::ReadOnly)){

        auto const json_doc = QJsonDocument::fromJson(QTextStream(&file_obj).readAll().toUtf8());
        if(json_doc.isNull()){
            return {};
        }
        if(!json_doc.isObject()){
            return {};
        }

        return json_doc.object();
    }

    return {};
}

void write_file_to_json(const QJsonObject &obj, const QString &save_as)
{
    if(QFile save_file(save_as); save_file.open(QIODevice::WriteOnly)){
        save_file.write(QString(QJsonDocument(obj).toJson()).toUtf8());
    }
}

void write_file_to_json(const QJsonArray &obj, const QString &save_as)
{
    if(QFile save_file(save_as); save_file.open(QIODevice::WriteOnly)){
        save_file.write(QString(QJsonDocument(obj).toJson()).toUtf8());
    }
}



}
