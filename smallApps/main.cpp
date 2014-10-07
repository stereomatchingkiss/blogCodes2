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
    out<<month_table.at(log.yy_mm_dd_.mm_)<<" "<<log.yy_mm_dd_.dd_<<", ";
    out<<log.yy_mm_dd_.yy_;//<<"   "<<std::left<<std::setw(7)<<log.commit_user_<<" ";

    std::string const Space("                      ");
    for(size_t i = 0; i != log.commit_files_.size(); ++i){
        if(i != 0){
            out<<Space<<" * "<<log.commit_files_[i].substr(5)<<"\n";
        }else{
            out<<"* "<<log.commit_files_[i].substr(5)<<"\n";
        }
    }

    for(auto const &Comment : log.commit_comments_){
        out<<Space<<" "<<Comment<<"\n";
    }
    out<<"\n";
}

int main(int argc, char const *argv[])
{
    try{
        std::map<size_t, std::string> const MonthTable
        {
            {1, "Jan"}, {2, "Feb"}, {3, "Mar"}, {4, "Apr"},
            {5, "May"}, {6, "Jun"}, {7, "Jul"}, {8, "Aug"},
            {9, "Sep"}, {10, "Oct"}, {11, "Nov"}, {12, "Dec"}
        };

        std::string const InFile = argc > 1 ? argv[1] : "log.txt";
        std::string const OutFile = argc > 2 ? argv[2] : "out.txt";
        std::ofstream out(OutFile);
        for(auto const &Log : parseSVNLog().parse_logs(InFile)){
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
