#ifndef CONFIG_UTILS_HPP
#define CONFIG_UTILS_HPP

class QRectF;
class QJsonArray;
class QPolygonF;

QPolygonF qjson_array_to_qpolygonf(QJsonArray const &input);
QRectF qjson_array_to_qrectf(QJsonArray const &val);

QJsonArray qpolygon_to_qjson_array(QPolygonF const &input);
QJsonArray qrectf_to_qjson_array(QRectF const &val);

#endif // CONFIG_UTILS_HPP
