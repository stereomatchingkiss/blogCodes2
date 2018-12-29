#ifndef GENERAL_SETTINGS_PARAM_PARSER_HPP
#define GENERAL_SETTINGS_PARAM_PARSER_HPP

#include <QString>

class QJsonObject;

class general_settings_param_parser
{
public:
    explicit general_settings_param_parser(QString const &setting_file_location);
    explicit general_settings_param_parser(QJsonObject const &input);

    double get_detect_confidence() const;
    QString get_mode() const;
    QString get_model_params() const;
    QString get_model_symbols() const;

private:
    double detect_confidence_ = 1.0;
    QString mode_;
    QString model_params_;
    QString model_symbols_;
};

#endif // GENERAL_SETTINGS_PARAM_PARSER_HPP
