#ifndef APPSTATISTIC_HPP
#define APPSTATISTIC_HPP

/*! \file appStatistic.h
    \brief do some statistic jobs base on the data\n
    collected by the python psutil
*/

#include <string>

void app_statistic(std::string const &file_name);

void cpu_usage_statistic(std::string const &file_name);

#endif // APPSTATISTIC_HPP
