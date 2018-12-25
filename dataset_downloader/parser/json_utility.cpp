#include "json_utility.hpp"

#include <QDebug>

#include <QFile>
#include <QTextStream>

#include <QJsonDocument>
#include <QJsonParseError>

QJsonDocument json_file_to_doc(const QString &file_location)
{
    QFile file(file_location);
    if(file.open(QIODevice::ReadOnly)){
        QJsonParseError error;
        QTextStream stream(&file);
        stream.setAutoDetectUnicode(true);
        auto const doc = QJsonDocument::fromJson(stream.readAll().toUtf8(), &error);
        if(error.error != QJsonParseError::NoError){
            qDebug()<<__func__<<"json parse error:"<<error.errorString();
        }

        return doc;
    }

    return {};
}

QJsonDocument json_string_to_doc(const QString &content)
{
    QJsonParseError error;
    auto const doc = QJsonDocument::fromJson(content.toUtf8(), &error);
    if(error.error != QJsonParseError::NoError){
        qDebug()<<__func__<<"json parse error:"<<error.errorString();
    }

    return doc;
}

std::vector<QJsonDocument> json_file_to_docs(const QString &file_location)
{
    QFile file(file_location);
    if(file.open(QIODevice::ReadOnly)){
        QJsonParseError error;
        QTextStream stream(&file);
        stream.setAutoDetectUnicode(true);
        std::vector<QJsonDocument> results;
        while(!stream.atEnd()){
            auto doc = QJsonDocument::fromJson(stream.readLine().toUtf8(), &error);
            if(error.error == QJsonParseError::NoError){
                results.emplace_back(std::move(doc));
            }else{
                qDebug()<<__func__<<"json parse error:"<<error.errorString();
            }
        }

        return results;
    }

    return {};
}
