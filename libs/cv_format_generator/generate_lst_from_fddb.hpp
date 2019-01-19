#ifndef GENERATE_LST_FROM_FDDB_HPP
#define GENERATE_LST_FROM_FDDB_HPP

#include <QFileInfoList>
#include <QJsonObject>
#include <QString>

class generate_lst_from_fddb
{
public:
    explicit generate_lst_from_fddb(QString const &json_location);

    void apply();

private:
    QJsonObject json_obj_;
};

#endif // GENERATE_LST_FROM_FDDB_HPP
