#include "generateNewSVNLog.hpp"
#include "parseSVNLog.hpp"
#include "svnLogStructure.hpp"

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/fusion/adapted/boost_tuple.hpp>
#include <boost/fusion/algorithm.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/utility/enable_if.hpp>

#include <fstream>
#include <iomanip>
#include <map>

namespace{

enum class NameTag
{
    HasName,
    NoName
};

/**
 * @brief parse_user_name from the commit message of svn
 * @param input commit messages
 * @return user name
 * @warning under experiment
 */
std::pair<std::string, std::vector<NameTag>>
parse_user_name(std::vector<std::string> const &input)
{
    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;

    using Rule = qi::rule<decltype(std::begin(input[0])), std::vector<std::string>()>;

    std::vector<std::string> names;
    std::vector<NameTag> valid_names;
    Rule commit_user = *(qi::omit[*qi::alnum] >> qi::blank
                                              >> (qi::omit[qi::uint_] >> -qi::omit[',']) % qi::blank
            >> *~qi::char_('\n'));
    for(auto const &Str : input){
        auto it = std::begin(Str);
        if(qi::parse(it, std::end(Str),
                     commit_user,
                     names) && it == std::end(Str)){
            //std::cout<<input[i]<<std::endl;
            valid_names.emplace_back(NameTag::HasName);
        }
        valid_names.emplace_back(NameTag::NoName);
    }

    //deal with the cases of multiple users commit, like
    //"Mar 12, 2015 Apple"
    //"Mar 12, 2015 Orange"
    std::string result;
    for(auto &str : names){
        boost::algorithm::trim(str);
        if(!str.empty()){
            result += str;
            //std::cout<<"names : "<<str<<std::endl;
            result += '&';
        }
    }
    if(!result.empty()){
        result.pop_back();
    }

    return {result, valid_names};
}

void generate_changes_log(std::ostream &out,
                          svnLogStructure const &log,
                          std::map<size_t, std::string> const &month_table)
{
    auto const NamesAndIndex = parse_user_name(log.commit_comments_);
    if(boost::iequals(NamesAndIndex.first, "NWTHAM")){
        //std::cout<<"name == "<<NamesAndIndex.first<<std::endl;
        out<<month_table.at(log.yy_mm_dd_.mm_)<<" "<<log.yy_mm_dd_.dd_<<", ";
        out<<log.yy_mm_dd_.yy_<<"   ";
        //auto const NamesAndIndex = parse_user_name(log.commit_comments_);
        out<<std::left<<std::setw(7)<<NamesAndIndex.first<<" ";

        std::string const Space("                      ");
        for(size_t i = 0; i != log.commit_files_.size(); ++i){
            std::string temp = log.commit_files_[i];
            boost::replace_first(temp, "scada/trunk", "scada");
            if(i != 0){
                out<<Space<<" * "<<temp.substr(5)<<"\n";
            }else{
                out<<"* "<<temp.substr(5)<<"\n";
            }
        }

        //std::cout<<NamesAndIndex<<std::endl;
        for(size_t i = 0; i != log.commit_comments_.size(); ++i){
            if(NamesAndIndex.second[i] != NameTag::HasName){
                out<<Space<<" "<<log.commit_comments_[i]<<"\n";
            }
        }
        out<<"\n";
    }
}

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
    //std::cin.get();
}
