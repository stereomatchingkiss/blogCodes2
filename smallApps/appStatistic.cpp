#include "appStatistic.hpp"

#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/max.hpp>
#include <boost/accumulators/statistics/mean.hpp>
#include <boost/accumulators/statistics/min.hpp>
#include <boost/accumulators/statistics/moment.hpp>
#include <boost/accumulators/statistics/variance.hpp>

#include <fstream>
#include <iostream>
#include <math.h>
#include <vector>

void app_statistic(const std::string &file_name)
{
    using namespace boost::accumulators;

    using Stats = features<tag::mean,
    tag::max, tag::min, tag::variance>;

    std::cout<<"app statistic : "<<file_name<<std::endl;

    std::ifstream in(file_name);
    double data{};
    accumulator_set<double, Stats> acc;
    while(in>>data){
        acc(data);
    }

    std::cout<<max(acc)<<std::endl;
    std::cout<<min(acc)<<std::endl;
    std::cout<<mean(acc)<<std::endl;
    std::cout<<std::sqrt(variance(acc))<<std::endl;
}


void cpu_usage_statistic(const std::string &file_name)
{
    using namespace boost::accumulators;

    using Stats = features<tag::mean,
    tag::max, tag::min, tag::variance>;

    std::cout<<"cpu_usage_statistic : "<<file_name<<std::endl;

    std::ifstream in(file_name);
    std::vector<double> data(8);
    std::vector<accumulator_set<double, Stats>> acc(data.size());
    while(in>>data[0]>>data[1]>>data[2]>>data[3]>>
          data[4]>>data[5]>>data[6]>>data[7]){
        for(size_t i = 0; i != data.size(); ++i){
            acc[i](data[i]);
        }
    }

    for(size_t i = 0; i != acc.size(); ++i){
        std::cout<<"cpu "<<i<<std::endl;
        std::cout<<max(acc[i])<<std::endl;
        std::cout<<min(acc[i])<<std::endl;
        std::cout<<mean(acc[i])<<std::endl;
        std::cout<<std::sqrt(variance(acc[i]))<<std::endl;
    }
}
