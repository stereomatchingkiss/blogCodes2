#include "generate_labels_from_kaggle_dataset.hpp"

#include <cv_format_generator/generate_lst_from_fddb.hpp>
#include <cv_format_generator/generate_lst_from_label_image.hpp>
#include <cv_format_generator/generate_lst_from_pascal_voc.hpp>
#include <cv_format_parser/fddb_parser.hpp>
#include <cv_format_parser/json_utility.hpp>

#include <QDebug>
#include <QGuiApplication>
#include <QJsonDocument>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

int main(int argc, char *argv[])try
{
    QGuiApplication a(argc, argv);

    if(argc != 2){
        qDebug()<<"Must enter the location of json";
        return -1;
    }

    auto const json_obj = json_file_to_doc(argv[1]).object();

    if(json_obj["mode"] == "kaggle_data_to_label_image"){
        generate_labels_from_kaggle_dataset(argv[1]).apply();
    }else if(json_obj["mode"] == "label_image_to_lst"){
        generate_lst_from_label_image(argv[1]).apply();
    }else if(json_obj["mode"] == "fddb_to_lst"){
        generate_lst_from_fddb(argv[1]).apply();
    }else if(json_obj["mode"] == "pascal_voc_to_lst"){
        generate_lst_from_pascal_voc(argv[1]).apply();
    }

}catch(std::exception const &ex){
    qDebug()<<__func__<<ex.what();
}
