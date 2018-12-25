#ifndef KAGGLE_FACE_DETECTION_PARSER_HPP
#define KAGGLE_FACE_DETECTION_PARSER_HPP

#include <QPointF>
#include <QString>

#include <vector>

class kaggle_face_detection_parser
{
public:
    struct parse_result
    {
        struct block{
            struct annotation
            {
                QPointF top_left_;
                QPointF bottom_right_;
                int width_ = 0;
                int height_ = 0;
            };

            std::vector<annotation> annos_;
            QString content_;
        };

        std::vector<block> blocks_;
    };

    parse_result parse(QString const &file_location) const;
};

#endif // KAGGLE_FACE_DETECTION_PARSER_HPP
