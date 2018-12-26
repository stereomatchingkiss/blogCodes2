#ifndef JSON_UTILITY_HPP
#define JSON_UTILITY_HPP

#include <QString>

#include <vector>

class QJsonDocument;

QJsonDocument json_file_to_doc(QString const &file_location);
std::vector<QJsonDocument> json_file_to_docs(QString const &file_location);
QJsonDocument json_string_to_doc(QString const &content);

#endif // JSON_UTILITY_HPP
