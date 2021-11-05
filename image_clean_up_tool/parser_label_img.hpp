#ifndef PARSER_LABEL_IMG_HPP
#define PARSER_LABEL_IMG_HPP

#include <QRectF>
#include <QSize>
#include <QString>

#include <vector>

class QDomElement;

class parser_label_img
{
public:
    struct parse_data
    {
        struct object
        {
            QString label_;
            QRectF rect_;            
        };

        QString abs_file_path_;
        std::vector<object> objects_;
        QSize sizes_;

        void print() const;
    };

    parser_label_img();

    parse_data parse(QString const &fname) const;
};

#endif // PARSER_LABEL_IMG_HPP
