#include "general_settings_param_parser.hpp"

#include <cv_format_parser/json_utility.hpp>

#include <QDir>
#include <QFileInfo>

#include <QJsonDocument>
#include <QJsonObject>

void general_settings_param_parser::initialize(QJsonObject const &input)
{
    detect_confidence_ = input["detect_confidence"].toDouble();
    mode_ = input["mode"].toString();
    model_params_ = input["model_params"].toString();
    model_symbols_ = input["model_symbols"].toString();
    process_size_ = cv::Size(input["detector_process_width"].toInt(), input["detector_process_height"].toInt());
}

general_settings_param_parser::general_settings_param_parser(const QString &setting_file_location)
{
    auto const doc = json_file_to_doc(setting_file_location);
    if(doc.isEmpty()){
        throw std::runtime_error(QString("%1 cannot parsed").arg(setting_file_location).toStdString());
    }

    auto const json_obj = doc.object();
    initialize(json_obj);
}

general_settings_param_parser::general_settings_param_parser(const QJsonObject &input)
{
    initialize(input);
}

double general_settings_param_parser::get_detect_confidence() const
{
    return detect_confidence_;
}

QString general_settings_param_parser::get_mode() const
{
    return mode_;
}

QString general_settings_param_parser::get_model_params() const
{
    return model_params_;
}

QString general_settings_param_parser::get_model_symbols() const
{
    return model_symbols_;
}

cv::Size general_settings_param_parser::get_detector_process_size() const
{
    return process_size_;
}
