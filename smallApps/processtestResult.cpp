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

void processPutGetTestResult::generate_change_log(const std::string &log_name)
{
    std::ifstream in(log_name);
    if(!in){
        std::cerr<<"can' t open file : "<<log_name<<std::endl;
        return;
    }

    std::ofstream out("out.log");
    if(!out){
        std::cerr<<"ca't open output file"<<std::endl;
    }

    //while(!in.eof()){
    auto const Log = read_change_log(in);
    proces_change_log(Log, out);
    //}
}

void
processPutGetTestResult::proces_change_log(processPutGetTestResult::logStructure const &log,
                                           std::ofstream &out)
{
    out<<log.commit_month_<<" "<<log.commit_day_<<", ";
    out<<log.commit_year_<<"   "<<log.user_name_<<" ";

    std::string const Space("                      ");
    for(size_t i = 0; i != log.commit_files_.size(); ++i){
        if(i != 0){
            out<<Space<<"* "<<log.commit_files_[i].substr(5)<<std::endl;
        }else{
            out<<"* "<<log.commit_files_[i].substr(5)<<std::endl;
        }
    }
    //std::cout<<"message size : "<<log.commit_messages_.size()<<std::endl;
    for(auto const &Message : log.commit_messages_){
        out<<Space<<"* "<<Message<<std::endl;
    }
}

processPutGetTestResult::logStructure
processPutGetTestResult::read_change_log(std::ifstream &in)
{
    logStructure log;

    std::getline(in, log.dashs_);
    std::getline(in, log.revision_);
    std::getline(in, log.change_path_flag_);

    std::cout<<log.dashs_<<std::endl;
    std::cout<<log.revision_<<std::endl;
    std::cout<<log.change_path_flag_<<std::endl;
    if(log.change_path_flag_.empty()){
        return log;
    }

    for(std::string str; ;){
        std::getline(in, str);
        std::cout<<str<<std::endl;
        if(!str.empty()){
            log.commit_files_.emplace_back(std::move(str));
        }else{
            break;
        }
    }

    in>>log.commit_month_>>log.commit_day_>>log.commit_year_>>log.user_name_;
    std::cout<<log.commit_month_<<" "<<log.commit_day_<<" "<<log.commit_year_;
    std::cout<<"   "<<log.user_name_<<std::endl;

    std::string temp;
    std::getline(in, temp);

    for(std::string str;;){
        std::getline(in, str);
        std::cout<<str<<std::endl;
        if(!str.empty()){
            log.commit_messages_.emplace_back(std::move(str));
        }else{
            break;
        }
    }

    return log;
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
