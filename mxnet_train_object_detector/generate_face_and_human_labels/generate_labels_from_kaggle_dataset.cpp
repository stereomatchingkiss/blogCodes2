#include "generate_labels_from_kaggle_dataset.hpp"

#include "general_settings_param_parser.hpp"
#include "person_detector.hpp"

#include <QDebug>
#include <QDir>
#include <QFileInfo>

#include <QJsonDocument>
#include <QJsonObject>

#include <mxnet-cpp/MxNetCpp.h>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

#include <mxnet_cpp_object_detection/object_detector.hpp>
#include <mxnet_cpp_object_detection/object_detector_filter.hpp>

#include <cv_format_parser/json_utility.hpp>

generate_labels_from_kaggle_dataset::generate_labels_from_kaggle_dataset(const QString &setting_file_location) :
    setting_file_location_(setting_file_location)
{

}

generate_labels_from_kaggle_dataset::~generate_labels_from_kaggle_dataset()
{

}

void generate_labels_from_kaggle_dataset::apply()
{
    auto const doc = json_file_to_doc(setting_file_location_);
    if(doc.isEmpty()){
        throw std::runtime_error(QString("%1 cannot parsed").arg(setting_file_location_).toStdString());
    }

    json_obj_ = doc.object();
    general_parser_ = std::make_unique<general_settings_param_parser>(json_obj_);

    if(json_obj_["mode"] == "kaggle_data_to_label_image"){

        auto const face_detection_json = json_obj_["face_detection_json_of_kaggle"].toString();
        auto const folder_to_save_xml = json_obj_["folder_to_save_xml"].toString();

        QDir().mkpath(folder_to_save_xml);

        kaggle_face_detection_parser kaggle_face_parser;
        auto const kaggle_face_results = kaggle_face_parser.parse(face_detection_json);

        person_detector_ = std::make_unique<person_detector>(setting_file_location_);

        folder_of_images_ = json_obj_["folder_of_kaggle_images"].toString();
        for(auto const &kaggle_block : kaggle_face_results.blocks_){
            label_image_generator::data_to_generate dg;
            append_face_annotaion(dg, kaggle_block);
            append_person_annotation(dg);

            label_image_generator().write_data_to_file(dg, folder_to_save_xml + "/" +
                                                       QFileInfo(dg.abs_path_).completeBaseName() + ".xml");
        }
    }
}

void generate_labels_from_kaggle_dataset::
append_face_annotaion(label_image_generator::data_to_generate &dg,
                      const kaggle_face_detection_parser::parse_result::block &kaggle_block) const
{
    dg.abs_path_ = folder_of_images_ + "/" + QFileInfo(kaggle_block.content_).fileName();
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
}

void generate_labels_from_kaggle_dataset::append_person_annotation(label_image_generator::data_to_generate &dg)
{
    cv::Mat const img = cv::imread(dg.abs_path_.toStdString(), cv::IMREAD_COLOR);
    if(img.empty()){
        throw std::runtime_error(QString("cannot open image:%1").arg(dg.abs_path_).toStdString());
    }

    for(auto const &obj_det_result : person_detector_->detect(img)){
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
}
