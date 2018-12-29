#include "general_settings_param_parser.hpp"

#include <cv_format_parser/json_utility.hpp>

#include <QDir>
#include <QFileInfo>

#include <QJsonDocument>
#include <QJsonObject>

general_settings_param_parser::general_settings_param_parser(const QString &setting_file_location)
{
    auto const doc = json_file_to_doc(setting_file_location);
    if(doc.isEmpty()){
        throw std::runtime_error(QString("%1 cannot parsed").arg(setting_file_location).toStdString());
    }

    auto const json_obj = doc.object();
    detect_confidence_ = json_obj["detect_confidence"].toDouble();
    mode_ = json_obj["mode"].toString();
    model_params_ = json_obj["model_params"].toString();
    model_symbols_ = json_obj["model_symbols"].toString();
}

general_settings_param_parser::general_settings_param_parser(const QJsonObject &input) :
    detect_confidence_(input["detect_confidence"].toDouble()),
    mode_(input["mode"].toString()),
    model_params_(input["model_params"].toString()),
    model_symbols_(input["model_symbols"].toString())
{
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
