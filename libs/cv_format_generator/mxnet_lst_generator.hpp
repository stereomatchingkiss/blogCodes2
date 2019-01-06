#ifndef MXNET_LST_GENERATOR_HPP
#define MXNET_LST_GENERATOR_HPP

#include <QFile>
#include <QPointF>
#include <QString>
#include <QTextStream>

#include <vector>

class mxnet_lst_generator
{
public:
    struct data_to_generate
    {
        QString img_path_;
        size_t index_ = 0;
        int width_ = 0;
        int height_ = 0;
        struct label
        {
            QPointF bottom_right_;
            size_t id_ = 0;
            QPointF top_left_;
        };

        std::vector<label> labels_;
    };

    explicit mxnet_lst_generator(QString const &save_as);

    void append(data_to_generate input);
    void apply();
    void apply(data_to_generate const &input);

private:
    std::vector<data_to_generate> data_to_gen_;
    QFile file_;
    QTextStream stream_;
};

#endif // MXNET_LST_GENERATOR_HPP
