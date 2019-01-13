#ifndef GENERATE_LST_FROM_PASCAL_VOC_HPP
#define GENERATE_LST_FROM_PASCAL_VOC_HPP

#include <QJsonObject>
#include <QString>

#include <set>

/**
 * @warning This class only generate the data related to "person" right now
 */
class generate_lst_from_pascal_voc
{
public:
    explicit generate_lst_from_pascal_voc(QString const &json_location);

    void apply();

private:
    std::set<QString> get_person_for_validate() const;

    QJsonObject json_obj_;
};

#endif // GENERATE_LST_FROM_PASCAL_VOC_HPP
