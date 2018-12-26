#ifndef LABEL_IMAGE_PARSER_HPP
#define LABEL_IMAGE_PARSER_HPP

#include <QPointF>
#include <QString>

#include <vector>

class QDomElement;

class label_image_parser
{
public:
    struct parse_result
    {
        struct object
        {
            QPointF bottom_right_;
            QString name_;
            QPointF top_left_;
        };

        QString abs_path_;
        int height_ = 0;
        std::vector<object> objects_;
        int width_ = 0;
    };

    parse_result parse(QString const &file_location) const;
    std::vector<parse_result> parse(QStringList const &files_location) const;

private:
    void parse_bndbox(parse_result::object &inout, QDomElement const &component) const;
    void parse_object(parse_result &inout, QDomElement const &component) const;
    void parse_width_height(parse_result &inout, QDomElement const &component) const;
};

#endif // LABEL_IMAGE_PARSER_HPP
