#ifndef PASCAL_VOC_PARSER_HPP
#define PASCAL_VOC_PARSER_HPP

#include <QPointF>
#include <QString>

#include <vector>

class QDomElement;

class pascal_voc_parser
{
public:
    struct parse_result
    {
        struct object
        {
            QString act_;
            QPointF bottom_right_;
            QString name_;
            QPointF top_left_;
        };

        QString file_name_;
        QString folder_;
        int height_ = 0;
        std::vector<object> objects_;
        int width_ = 0;
    };

    parse_result parse(QString const &file_location) const;

private:
    void parse_action(parse_result::object &inout, QDomElement const &component) const;
    void parse_bndbox(parse_result::object &inout, QDomElement const &component) const;
    void parse_object(parse_result &inout, QDomElement const &component) const;
    void parse_width_height(parse_result &inout, QDomElement const &component) const;
};

#endif // PASCAL_VOC_PARSER_HPP
