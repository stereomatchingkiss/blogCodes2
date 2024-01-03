#pragma once

#include <QString>

class QJsonArray;
class QJsonObject;

namespace flt::json{

QJsonObject parse_file_to_jobj(QString const &fname);

void write_file_to_json(QJsonArray const &obj, QString const &save_as);
void write_file_to_json(QJsonObject const &obj, QString const &save_as);

}
