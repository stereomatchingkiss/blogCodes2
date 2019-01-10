#ifndef FDDB_PARSER_HPP
#define FDDB_PARSER_HPP

#include <QPointF>
#include <QString>

#include <vector>

/**
 * This parser assume the folders "FDDB-folds", "2002" and "2003"
 * are inside the same folder
 */
class fddb_parser
{
public:
    struct parse_result
    {
        struct block{
            struct annotation
            {
                QPointF top_left_;
                QPointF bottom_right_;
            };

            std::vector<annotation> annos_;
            QString img_path_;
            int width_ = 0;
            int height_ = 0;
        };

        std::vector<block> blocks_;
    };

    /**
     * @param folder_location Location of the folder contain "FDDB-folds", "2002" and "2003"
     */
    parse_result parse(QString const &folder_location) const;

private:
    fddb_parser::parse_result::block parse_block(int &i,
                                                 QStringList const &contents,
                                                 QString const &folder_location) const;
};

#endif // FDDB_PARSER_HPP
