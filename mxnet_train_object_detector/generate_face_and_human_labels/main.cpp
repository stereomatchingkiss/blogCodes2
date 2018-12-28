#include <QDebug>
#include <QDir>
#include <QFileInfo>

#include <QJsonDocument>
#include <QJsonObject>

#include <mxnet-cpp/MxNetCpp.h>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

#include <object_detector.hpp>
#include <object_detector_filter.hpp>

#include <cv_format_generator/label_image_generator.hpp>
#include <cv_format_parser/json_utility.hpp>
#include <cv_format_parser/kaggle_face_detection_parser.hpp>

int main(int argc, char *argv[])
{
    if(argc != 2){
        qDebug()<<"Must enter the location of json";
        return -1;
    }

    auto const doc = json_file_to_doc(argv[1]);
    auto const json_obj = doc.object();

    auto const face_detection_json = json_obj["face_detection_json_of_kaggle"].toString();
    auto const folder_to_save_xml = json_obj["folder_to_save_xml"].toString();
    auto const folder_of_images = json_obj["folder_of_kaggle_images"].toString();

    QDir().mkpath(folder_to_save_xml);

    kaggle_face_detection_parser kaggle_face_parser;
    auto const kaggle_face_results = kaggle_face_parser.parse(face_detection_json);

    double const confidence = json_obj["detect_confidence"].toDouble();
    auto const process_size = cv::Size(608, 608);
    object_detector obj_detector(json_obj["model_params"].toString().toStdString(),
                                 json_obj["model_symbols"].toString().toStdString(),
                                 mxnet::cpp::Context::gpu(0),
                                 process_size);

    std::vector<object_detector_filter::item_type> types_to_detect;
    types_to_detect.emplace_back(object_detector_filter::item_type::person);
    object_detector_filter filter(types_to_detect, process_size, cv::Size(1, 1), confidence);

    for(auto const &kaggle_block : kaggle_face_results.blocks_){
        label_image_generator::data_to_generate dg;
        dg.abs_path_ = folder_of_images + "/" + QFileInfo(kaggle_block.content_).fileName();
        qDebug()<<"processing image:"<<dg.abs_path_;
        for(auto const &anno : kaggle_block.annos_){
            dg.height_ = anno.height_;
            dg.width_ = anno.width_;
            label_image_generator::data_to_generate::object obj;
            obj.name_ = "face";
            obj.top_left_.setX(anno.top_left_.x() * dg.width_);
            obj.top_left_.setY(anno.top_left_.y() * dg.height_);
            obj.bottom_right_.setX(anno.bottom_right_.x() * dg.width_);
            obj.bottom_right_.setY(anno.bottom_right_.y() * dg.height_);

            dg.objects_.emplace_back(obj);
        }

        cv::Mat const img = cv::imread(dg.abs_path_.toStdString(), cv::IMREAD_COLOR);
        obj_detector.forward(img);
        filter.set_image_size(cv::Size(img.cols, img.rows));
        auto const obj_det_results = filter.filter(obj_detector.get_outputs());
        for(auto const &obj_det_result : obj_det_results){
            dg.height_ = img.rows;
            dg.width_ = img.cols;
            label_image_generator::data_to_generate::object obj;
            obj.name_ = "person";
            obj.top_left_.setX(obj_det_result.roi_.tl().x);
            obj.top_left_.setY(obj_det_result.roi_.tl().y);
            obj.bottom_right_.setX(obj_det_result.roi_.br().x);
            obj.bottom_right_.setY(obj_det_result.roi_.br().y);

            dg.objects_.emplace_back(obj);
        }

        label_image_generator().write_data_to_file(dg, folder_to_save_xml + "/" +
                                                   QFileInfo(dg.abs_path_).completeBaseName() + ".xml");
    }


}
