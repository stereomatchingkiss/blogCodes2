#ifndef GENERATE_LABELS_FROM_KAGGLE_DATASET_HPP
#define GENERATE_LABELS_FROM_KAGGLE_DATASET_HPP

#include <cv_format_generator/label_image_generator.hpp>
#include <cv_format_parser/kaggle_face_detection_parser.hpp>

#include <opencv2/core.hpp>

#include <QJsonObject>
#include <QString>

#include <memory>

class object_detector;
class object_detector_filter;

class general_settings_param_parser;

class generate_labels_from_kaggle_dataset
{
public:
    explicit generate_labels_from_kaggle_dataset(QString const &setting_file_location);
    ~generate_labels_from_kaggle_dataset();

    void apply();

private:
    void append_face_annotaion(label_image_generator::data_to_generate &dg,
                               kaggle_face_detection_parser::parse_result::block const &kaggle_block) const;
    void append_person_annotation(label_image_generator::data_to_generate &dg);

    std::unique_ptr<object_detector_filter> filter_;
    QString folder_of_images_;
    std::unique_ptr<general_settings_param_parser> general_parser_;
    QJsonObject json_obj_;
    std::unique_ptr<object_detector> object_detector_;
    cv::Size const process_size_ = cv::Size(608, 608);
    QString setting_file_location_;
};

#endif // GENERATE_LABELS_FROM_KAGGLE_DATASET_HPP
