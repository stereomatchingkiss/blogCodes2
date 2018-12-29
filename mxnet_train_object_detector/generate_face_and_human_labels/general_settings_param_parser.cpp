#include "general_settings_param_parser.hpp"

#include <QJsonObject>

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
