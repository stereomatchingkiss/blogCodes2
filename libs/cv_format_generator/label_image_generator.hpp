#ifndef LABEL_IMAGE_GENERATOR_HPP
#define LABEL_IMAGE_GENERATOR_HPP

#include <QDomDocument>
#include <QPointF>
#include <QString>

#include <vector>

class label_image_generator
{
public:
    struct data_to_generate
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

    void write_data_to_file(data_to_generate const &input, QString const &file_location);

private:
    void append_objects();
    void append_path();
    void append_size();
    void append_source();

    QDomElement create_child(QString const &tag_name, QString const &node_value);

    data_to_generate data_to_gen_;
    QDomDocument doc_;
    QDomElement root_;
};

#endif // LABEL_IMAGE_GENERATOR_HPP
