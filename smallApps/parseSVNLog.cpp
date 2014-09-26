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
    bool r = qi::parse(begin, end,
                       grammar,
                       output);

    return !r || begin != end;
}

}

namespace{

template <typename Iterator>
struct logGrammar : qi::grammar<Iterator, std::vector<parseSVNLog::logStructure>()>
{
    logGrammar() : logGrammar::base_type(result_)
    {
        using qi::repeat;

        dash_ = *qi::omit["-"] >> qi::eol;
        revision_ = *~qi::char_('\n') >> qi::eol;
        change_path_tag_ = qi::omit[*~qi::char_('\n')] >> qi::eol;
        commit_files_ = *(qi::blank >> *~qi::char_('\n') >>
                        qi::eol) >> qi::eol;
        commit_month_ = *qi::alpha >> qi::blank;
        commit_day_ = qi::int_ >> qi::blank;
        commit_year_ = qi::int_ >> qi::blank;
        commit_user_ = *~qi::char_('\n') >> qi::eol;
        commit_comments_ = *(qi::char_('-') >> ~qi::char_('\n') >> qi::eol)
                           >> +qi::eol;
        log_struct_ = dash_ >> revision_ >> change_path_tag_ >> commit_files_
                        >> commit_month_ >> commit_day_ >> commit_year_
                        >> commit_user_ >> commit_comments_;
        result_ = *log_struct_;
    }

    qi::rule<Iterator, void()> dash_;
    qi::rule<Iterator, std::string()> revision_;
    qi::rule<Iterator, void()> change_path_tag_;
    qi::rule<Iterator, std::vector<std::string>()> commit_files_;
    qi::rule<Iterator, std::string()> commit_month_;
    qi::rule<Iterator, size_t()> commit_day_;
    qi::rule<Iterator, size_t()> commit_year_;
    qi::rule<Iterator, std::string()> commit_user_;
    qi::rule<Iterator, std::string()> commit_comments_;
    qi::rule<Iterator, parseSVNLog::logStructure()> log_struct_;
    qi::rule<Iterator, std::vector<parseSVNLog::logStructure>()> result_;
};

}

BOOST_FUSION_ADAPT_STRUCT(
        parseSVNLog::logStructure,
        (std::string, revision_)
        (std::vector<std::string>, commit_files_)
        (std::string, commit_month_)
        (size_t, commit_day_)
        (size_t, commit_year_)
        (std::string, commit_user_name_)
        (std::vector<std::string>, commit_comments_)
        )

parseSVNLog::parseSVNLog()
{
}

std::vector<parseSVNLog::logStructure>
parseSVNLog::parse_logs(const std::string &file_name) const
{
    auto const Content = read_whole_file(file_name);
    std::cout<<Content<<std::endl<<std::endl;

    std::vector<logStructure> logs;
    logGrammar<std::string::const_iterator> grammar;
    parse_log_data(std::begin(Content), std::end(Content), grammar, logs);

    auto const &log = logs[0];
    std::cout<<log<<std::endl;

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
    std::cout<<log.revision_<<std::endl;
    for(auto const &Str : log.commit_files_){
        std::cout<<Str<<std::endl;
    }
    std::cout<<log.commit_month_<<" "<<log.commit_day_;
    std::cout<<" "<<log.commit_year_<<" "<<log.commit_user_name_<<std::endl;

    for(auto const &Str : log.commit_comments_){
        std::cout<<Str<<std::endl;
    }

    return out;
}
