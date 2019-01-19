#ifndef COCO_INSTANCE_PARSER_HPP
#define COCO_INSTANCE_PARSER_HPP

#include <QPointF>
#include <QString>

#include <map>
#include <vector>

/**
 * This parser only support coco format of 2017 and
 * expect the files organize as following
 * --parent
 *   --val2017
 *   --train2017
 *   --test2017
 *   --annotations
 */
class coco_instance_parser
{
public:
    struct parse_result
    {
        struct block{
            struct instance_info
            {
                QPointF bottom_right_;
                int category_id_ = 0;
                QPointF top_left_;
            };

            std::vector<instance_info> instances_;
            int width_ = 0;
            int height_ = 0;
        };

        std::map<QString, block> blocks_;
    };

    /**
     * @param file_location Location of the file "instances_train2017.json" or "instances_val2017.json"
     * @param image_location Location of images
     */
    parse_result parse(QString const &file_location, QString const &image_location);
};

#endif // COCO_INSTANCE_PARSER_HPP
