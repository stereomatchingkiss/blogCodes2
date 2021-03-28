#include "config_utils.hpp"

#include <QJsonArray>
#include <QPolygonF>
#include <QRectF>

QRectF qjson_array_to_qrectf(const QJsonArray &val)
{
    return QRectF(val[0].toDouble(), val[1].toDouble(), val[2].toDouble(), val[3].toDouble());
}

QJsonArray qrectf_to_qjson_array(const QRectF &val)
{
    return QJsonArray{val.x(), val.y(), val.width(), val.height()};
}

QPolygonF qjson_array_to_qpolygonf(const QJsonArray &input)
{
    QPolygonF results;
    for(auto const &val : input){
        auto const pt = val.toArray();
        results.push_back(QPointF(pt[0].toDouble(), pt[1].toDouble()));
    }

    return results;
}

QJsonArray qpolygon_to_qjson_array(const QPolygonF &input)
{
    QJsonArray obj;
    for(auto const &val : input){
        QJsonArray pt;
        pt.push_back(val.x());
        pt.push_back(val.y());
        obj.push_back(pt);
    }

    return obj;
}
