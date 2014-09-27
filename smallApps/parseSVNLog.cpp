#include "parseSVNLog.hpp"

#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/include/io.hpp>
#include <boost/fusion/adapted/std_pair.hpp> //without this, spirit can't accept the type with std::pair

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>

#include <algorithm>
#include <fstream>
#include <iterator>

namespace{

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;

template<typename Iterator, typename Grammar, typename T>
bool parse_log_data(Iterator begin, Iterator end, Grammar &grammar, T &output)
{
    auto const OriginBegin = begin;
    bool r = qi::parse(begin, end,
                       grammar,
                       output);

    if(!r || begin != end){
        std::cout<<"parse fail at "<<(begin - OriginBegin)<<std::endl;
        std::cout<<std::string(OriginBegin, begin)<<std::endl;
        std::cout<<"-----------success?-------------------"<<std::endl;
        return false;
    }

    return true;
}

}

namespace{

//using logGrammarType = parseSVNLog::logStructure;
using logGrammarType = std::vector<parseSVNLog::logStructure>;

template <typename Iterator>
struct logGrammar : qi::grammar<Iterator, logGrammarType()>
{
    logGrammar() : logGrammar::base_type(result_)
    {
        dash_ = *qi::eol>> qi::omit[*qi::char_("-")] >> +qi::eol;
        //r6249 | sysdev | 2014-09-26 15:54:24 +0800 (Fri, 26 Sep 2014) | 3 lines
        //revision_ = dash_ >> qi::omit["r"] >> qi::uint_ >> " | ";
        revision_ =  dash_ >> qi::omit["r"] >> qi::uint_ >> " | ";
        branch_ = *~qi::char_('|');
        commit_year_ = "| " >> qi::uint_;
        commit_month_ = "-" >> qi::uint_ >> "-";
        commit_day_ = qi::uint_ >> qi::omit[*~qi::char_('\n')];
        omit_strings_ = qi::repeat(2)[qi::omit[*~qi::char_('\n')] >> *qi::eol];
        commit_files_ = omit_strings_ >> *(qi::blank >> *~qi::char_('\n') >>
                                           qi::eol) >> *qi::eol;
        commit_user_ = qi::omit[*qi::alnum] >> qi::blank
                                            >> qi::omit[qi::uint_] % qi::blank
                                            >> *~qi::char_('\n') >> qi::eol;
        commit_comments_ = *(!qi::eol >> *~qi::char_("\n") >> qi::eol);

        /*std::cout<<qi::parse(std::begin(Text), std::end(Text),
                             *(!qi::eol >> *~qi::char_("\n") >> qi::eol),
                             comments)<<std::endl;*/

        /*omit_strings_ = qi::repeat(2)[qi::omit[*~qi::char_('\n')] >> *qi::eol];
        commit_files_ = omit_strings_ >> *(qi::blank >> *~qi::char_('\n') >>
                        qi::eol) >> *qi::eol;
        commit_month_ = *qi::alpha >> *qi::blank;
        commit_day_ = qi::uint_ >> -qi::omit[","] >> *qi::blank;
        commit_year_ = qi::uint_ >> *qi::blank;
        commit_user_ = *~qi::char_('\n') >> *qi::eol;
        commit_comments_ = *(!qi::eol >> *~qi::char_("\n") >> qi::eol);
        result_ = *(revision_ >> commit_files_
                        >> commit_month_ >> commit_day_ >> commit_year_
                        >> commit_user_ >> commit_comments_);*/
        result_ = *(revision_ >> branch_ >> commit_year_
                            >> commit_month_ >> commit_day_
                            >> commit_files_ >> commit_user_
                            >> commit_comments_);
    }

    qi::rule<Iterator, void()> dash_;
    qi::rule<Iterator, size_t()> revision_;
    qi::rule<Iterator, std::string()> branch_;
    qi::rule<Iterator, size_t()> commit_month_;
    qi::rule<Iterator, size_t()> commit_day_;
    qi::rule<Iterator, size_t()> commit_year_;
    qi::rule<Iterator, void()> omit_strings_;
    qi::rule<Iterator, std::vector<std::string>()> commit_files_;
    qi::rule<Iterator, std::string()> commit_user_;
    qi::rule<Iterator, std::vector<std::string>()> commit_comments_;
    /*qi::rule<Iterator, void()> omit_strings_;
    qi::rule<Iterator, std::vector<std::string>()> commit_files_;
    qi::rule<Iterator, std::string()> commit_month_;
    qi::rule<Iterator, size_t()> commit_day_;
    qi::rule<Iterator, size_t()> commit_year_;
    qi::rule<Iterator, std::string()> commit_user_;
    qi::rule<Iterator, std::vector<std::string>()> commit_comments_;*/
    qi::rule<Iterator, logGrammarType()> result_;
    //qi::rule<Iterator, std::vector<parseSVNLog::logStructure>()> result_;
};

}

BOOST_FUSION_ADAPT_STRUCT(
        parseSVNLog::logStructure,
        (size_t, revision_)
        (std::string, branch_)
        (size_t, commit_year_)
        (size_t, commit_month_)
        (size_t, commit_day_)
        (std::vector<std::string>, commit_files_)
        (std::string, commit_user_)
        (std::vector<std::string>, commit_comments_)
        )

parseSVNLog::parseSVNLog()
{
}

std::vector<parseSVNLog::logStructure>
parseSVNLog::parse_logs(const std::string &file_name) const
{
    /*std::string const Text = "r6249 | sysdev | 2014-09-26 15:54:24 +0800 (Fri, 26 Sep 2014) | 3 lines";
    size_t num = 0;
    std::string branch;
    size_t year, month, day;
    //commit_year_ = "| " >> qi::uint_;
    //commit_month_ = "-" >> qi::uint_ >> "-";
    //commit_day_ = qi::uint_ >> qi::omit[*~qi::char_('\n')] >> *qi::eol;
    std::cout<<qi::parse(std::begin(Text), std::end(Text),
                         qi::omit["r"] >> qi::uint_ >> " | "
            >> *~qi::char_('|')
            >> "| " >> qi::uint_
            >> "-" >> qi::uint_ >> "-"
            >> qi::uint_ >> qi::omit[*qi::char_],
            num, branch, year, month, day)<<std::endl;
    std::cout<<num<<", "<<branch<<", ";
    std::cout<<year<<", "<<month<<", "<<day<<std::endl;*/

    /*std::string const Text = "Sep 26 2014 NWTHAM";
    std::string user_name;
    std::cout<<qi::parse(std::begin(Text), std::end(Text),
                         qi::omit[*qi::alnum] >> qi::blank >>
               qi::omit[qi::uint_] % qi::blank >>
               *~qi::char_('\n') >> qi::eol,
            user_name)<<std::endl;
    std::cout<<"user name = "<<user_name<<std::endl;*/


    //commit_comments_ = *(!qi::eol >> *~qi::char_("\n") >> qi::eol);
    std::string const Text = "- refine macros name\n"
            "- use exception to handler errors of rtdb\n";
    std::vector<std::string> comments;
    std::cout<<qi::parse(std::begin(Text), std::end(Text),
                         *(!qi::eol >> *~qi::char_("\n") >> qi::eol),
                         comments)<<std::endl;
    for(auto const &Str : comments){
        std::cout<<Str<<std::endl;
    }

    auto const Content = read_whole_file(file_name);
    //std::cout<<Content<<std::endl<<std::endl;
    //std::cout<<Content.size()<<std::endl;

    logGrammarType logs;
    logGrammar<std::string::const_iterator> grammar;
    parse_log_data(std::begin(Content), std::end(Content), grammar, logs);

    //std::cout<<"log records : "<<logs<<std::endl;

    //std::cout<<"log records : "<<logs.size()<<std::endl;

    return logs;
}

std::string parseSVNLog::read_whole_file(const std::string &file_name) const
{
    std::ifstream in(file_name);
    if(!in){
        throw std::runtime_error("can not open file");
    }
    std::string content;
    std::copy(std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>(),
              std::back_inserter(content));

    return content;
}

std::ostream &operator<<(std::ostream &out, const parseSVNLog::logStructure &log)
{
    //out<<log.dashs_<<log.revision_<<log.change_path_flag_;
    std::cout<<log.revision_<<", "<<log.branch_<<std::endl;
    std::cout<<log.commit_year_<<"-"<<log.commit_month_<<"-";
    std::cout<<log.commit_day_<<std::endl;
    for(auto const &Str : log.commit_files_){
        std::cout<<Str<<std::endl;
    }
    std::cout<<"commit user : "<<log.commit_user_<<std::endl;

    for(auto const &Str : log.commit_comments_){
        std::cout<<Str<<std::endl;
    }

    return out;
}


parseSVNLog::logStructure::logStructure() :
    revision_(0),
    commit_year_(1000),
    commit_month_(0),
    commit_day_(32)
{}
