#include "processtestResult.hpp"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

processPutGetTestResult::processPutGetTestResult() :
    data_with_notify_(get_data("withNotification/put_performance_test.txt")),
    data_without_notify_(get_data("withoutNotification/put_performance_test.txt"))
{    
}

void processPutGetTestResult::average_time()
{

}

void processPutGetTestResult::compare_two_performance_test_file()
{
    std::ofstream out("diff.txt");
    size_t const Spaces = 50;
    out<<std::setw(Spaces)<<"put type ";
    out<<std::setw(Spaces)<<"elapsed diff(no notify - notify, ms) ";
    out<<std::setw(Spaces)<<"frequency diff(no notify - notify, ms) ";
    out<<std::setw(Spaces)<<"elapsed percentage(no notify / notify) ";
    out<<std::setw(Spaces)<<"frequency percentage(no notify / notify)"<<std::endl;
    for(size_t i = 0; i != data_with_notify_.elapse_.size(); ++i){
        out<<std::setw(Spaces)<<data_without_notify_.put_types_[i];
        out<<std::setw(Spaces)<<(data_without_notify_.elapse_[i] - data_with_notify_.elapse_[i]);
        out<<std::setw(Spaces)<<(data_without_notify_.frequency_[i] - data_with_notify_.frequency_[i]);
        out<<std::setw(Spaces)<<(data_without_notify_.elapse_[i] / data_with_notify_.elapse_[i] * 100);
        out<<std::setw(Spaces)<<(data_without_notify_.frequency_[i] / data_with_notify_.frequency_[i] * 100)<<std::endl;
    }
}

processPutGetTestResult::fileData processPutGetTestResult::get_data(const std::string &file_name)
{
    std::ifstream in(file_name);
    std::string message;
    std::getline(in, message);

    double elapsed = 0;
    double frequency = 0;
    int num = 0;
    fileData data;

    while(in>>num>>message>>elapsed>>frequency){
        data.elapse_.emplace_back(elapsed);
        data.frequency_.emplace_back(frequency);
        data.put_types_.emplace_back(message);
        std::cout<<message<<" : "<<elapsed<<" : "<<frequency<<std::endl;
    }

    return data;
}
