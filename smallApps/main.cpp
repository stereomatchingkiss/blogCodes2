#include "appStatistic.hpp"
#include "parseSVNLog.hpp"
#include "svnLogStructure.hpp"

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/spirit/include/qi.hpp>

#include <exception>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>

/**
 * @brief parse_user_name from the commit message of svn
 * @param input commit messages
 * @return user name
 * @warning under experiment
 */
std::pair<std::string, std::vector<char>>
parse_user_name(std::vector<std::string> const &input)
{
    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;

    using Rule = qi::rule<decltype(std::begin(input[0])), std::vector<std::string>()>;

    std::vector<std::string> names;
    std::vector<char> valid_names;
    Rule commit_user = *(qi::omit[*qi::alnum] >> qi::blank
                                              >> (qi::omit[qi::uint_] >> -qi::omit[',']) % qi::blank
            >> *~qi::char_('\n'));
    for(auto const &Str : input){
        auto it = std::begin(Str);
        if(qi::parse(it, std::end(Str),
                     commit_user,
                     names) && it == std::end(Str)){
            //std::cout<<input[i]<<std::endl;
            valid_names.emplace_back('1');
        }
        valid_names.emplace_back('0');
    }

    std::string result;
    for(auto &str : names){
        boost::algorithm::trim(str);
        if(!str.empty()){
            result += str;
            //std::cout<<"names : "<<str<<std::endl;
            result += '&';
        }
    }
    result.pop_back();

    return {result, valid_names};
}

void generate_changes_log(std::ostream &out,
                          svnLogStructure const &log,
                          std::map<size_t, std::string> const &month_table)
{
    out<<month_table.at(log.yy_mm_dd_.mm_)<<" "<<log.yy_mm_dd_.dd_<<", ";
    out<<log.yy_mm_dd_.yy_<<"   ";
    auto const NamesAndIndex = parse_user_name(log.commit_comments_);
    out<<std::left<<std::setw(7)<<NamesAndIndex.first<<" ";

    std::string const Space("                      ");
    for(size_t i = 0; i != log.commit_files_.size(); ++i){
        std::string temp = log.commit_files_[i];
        boost::replace_first(temp, "scada/trunk", "scada/");
        if(i != 0){
            out<<Space<<" * "<<temp.substr(5)<<"\n";
        }else{
            out<<"* "<<temp.substr(5)<<"\n";
        }
    }

    //std::cout<<NamesAndIndex<<std::endl;
    for(size_t i = 0; i != log.commit_comments_.size(); ++i){
        if(NamesAndIndex.second[i] != '1'){
            out<<Space<<" "<<log.commit_comments_[i]<<"\n";
        }
    }
    out<<"\n";
}

void generate_changes_log(int argc, char const *argv[])
{
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
        //std::cout<<Log<<std::endl;
        generate_changes_log(out, Log, MonthTable);
    }
}

/*void change_file_string(std::string const &file, std::string const &original,
                        std::string const &new_str)
{
    std::ifstream in(file);
    std::string temp;
    std::string result;
    while(std::getline(in, temp)){
        boost::replace_first(temp, original, new_str);
        result += temp;
    }

    std::ofstream out(file);
    out<<result;
}*/

int main(int argc, char const *argv[])
{
    try{        
        generate_changes_log(argc, argv);

        /*if(argc > 2){
            cpu_usage_statistic(argv[1]);
        }else if(argc > 1){
            app_statistic(argv[1]);
        }*/

    }catch(std::exception const &ex){
        std::cerr<<ex.what()<<std::endl;
    }

    return 0;
}
