#include "parseSVNLog.hpp"
//#include "processtestResult.hpp"
#include "svnLogStructure.hpp"

#include <exception>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>

void generate_changes_log(std::ostream &out,
                          svnLogStructure const &log,
                          std::map<size_t, std::string> const &month_table)
{
    out<<month_table.at(log.yy_mm_dd_.mm_)<<" "<<log.yy_mm_dd_.mm_<<", ";
    out<<log.yy_mm_dd_.dd_<<"   "<<std::left<<std::setw(7)<<log.commit_user_<<" ";

    std::string const Space("                      ");
    for(size_t i = 0; i != log.commit_files_.size(); ++i){
        if(i != 0){
            if(log.commit_files_[i].size() > 5){
                out<<Space<<" * "<<log.commit_files_[i].substr(5)<<"\n";
            }
        }else{
            //out<<"size : "<<log.commit_files_[i].size()<<std::endl;
            if(log.commit_files_[i].size() > 5){
                out<<"* "<<log.commit_files_[i].substr(5)<<"\n";
            }
        }
    }
    //std::cout<<"message size : "<<log.commit_messages_.size()<<std::endl;
    for(auto const &Comment : log.commit_comments_){
        out<<Space<<" "<<Comment<<"\n";
    }
    out<<"\n";
}

int main()
{
    try{
        parseSVNLog log;
        auto const Logs = log.parse_logs("log.txt");
        std::map<size_t, std::string> const MonthTable
        {
            {1, "Jan"}, {2, "Feb"}, {3, "Mar"}, {4, "Apr"},
            {5, "May"}, {6, "Jun"}, {7, "Jul"}, {8, "Aug"},
            {9, "Sep"}, {10, "Oct"}, {11, "Nov"}, {12, "Dec"}
        };
        std::ofstream out("out.log");
        for(auto const &Log : Logs){
            std::cout<<Log<<std::endl;
            generate_changes_log(out, Log, MonthTable);
        }

        //processPutGetTestResult process;
        //process.compare_two_performance_test_file();
        //process.generate_change_log("log.txt");
    }catch(std::exception const &ex){
        std::cerr<<ex.what()<<std::endl;
    }

    return 0;
}
