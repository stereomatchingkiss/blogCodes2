#include "generate_lst_from_fddb.hpp"

#include "../cv_format_parser/fddb_parser.hpp"
#include "../cv_format_parser/json_utility.hpp"

#include "mxnet_lst_generator.hpp"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QTextStream>

namespace{

QPointF normalize(QPointF const &input, int width, int height)
{
    QPointF result;
    result.setX(input.x() / width);
    result.setY(input.y() / height);

    return result;
}

}

generate_lst_from_fddb::generate_lst_from_fddb(const QString &json_location)
{
    json_obj_ = json_file_to_doc(json_location).object();
}

void generate_lst_from_fddb::apply()
{
    fddb_parser parser;
    mxnet_lst_generator lst_gen(json_obj_["save_lst_as"].toString());
    auto const parse_result = parser.parse(json_obj_["folder_of_fddb"].toString());
    for(auto const &block : parse_result.blocks_){
        mxnet_lst_generator::data_to_generate dtg;
        dtg.height_ = block.height_;
        dtg.width_ = block.width_;
        dtg.img_path_ = block.relative_img_path_;
        for(auto const &anno : block.annos_){
            mxnet_lst_generator::data_to_generate::label lb;
            lb.bottom_right_ = normalize(anno.bottom_right_, dtg.width_, dtg.height_);
            lb.top_left_ = normalize(anno.top_left_, dtg.width_, dtg.height_);
            lb.id_ = 0;
            dtg.labels_.emplace_back(std::move(lb));
        }
        lst_gen.apply(dtg);
    }
}
