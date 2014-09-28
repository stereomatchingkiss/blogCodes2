#include "parseSVNLog.hpp"
#include "svnLogStructure.hpp"

#include <boost/algorithm/string.hpp>

#include <boost/range.hpp>
#include <boost/range/algorithm.hpp>

#include <boost/spirit/include/qi.hpp>

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
        std::cerr<<"parse fail at "<<(begin - OriginBegin)<<std::endl;
        //std::cout<<std::string(OriginBegin, begin)<<std::endl;
        //std::cout<<"-----------success?-------------------"<<std::endl;
        return false;
    }

    return true;
}

using logGrammarType = std::vector<svnLogStructure>;

template <typename Iterator>
struct logGrammar : qi::grammar<Iterator, logGrammarType()>
{
    logGrammar() : logGrammar::base_type(result_)
    {
        dash_ = *qi::eol>> qi::omit[*qi::char_("-")] >> +qi::eol;
        revision_ =  dash_ >> "r" >> qi::uint_ >> " | ";
        branch_ = *~qi::char_('|');
        yy_mm_dd_ = "| " >> qi::uint_ >> "-" >> qi::uint_ >> "-"
                         >> qi::uint_ >> qi::blank;
        hh_mm_ss_ = qi::uint_ >> ":" >> qi::uint_ >> ":" >> qi::uint_;
        omit_strings_ = qi::repeat(2)[qi::omit[*~qi::char_('\n')] >> *qi::eol];
        commit_files_ = omit_strings_ >> *(qi::blank >> *~qi::char_('\n') >>
                                           qi::eol) >> *qi::eol;
        commit_user_ = (qi::omit[*qi::alnum] >> qi::blank
                                             >> (qi::omit[qi::uint_] >> -qi::omit[',']) % qi::blank
                >> *~qi::char_('\n') >> qi::eol) |
                *~qi::char_('\n') >> qi::eol;
        commit_comments_ = *(!qi::eol >> *~qi::char_("\n") >> qi::eol);

        result_ = *(revision_ >> branch_ >> yy_mm_dd_
                    >> hh_mm_ss_ >> commit_files_
                    >> commit_user_ >> commit_comments_);
    }

    qi::rule<Iterator, void()> dash_;
    qi::rule<Iterator, size_t()> revision_;
    qi::rule<Iterator, std::string()> branch_;    
    qi::rule<Iterator, yy_mm_dd()> yy_mm_dd_;
    qi::rule<Iterator, hh_mm_ss()> hh_mm_ss_;
    qi::rule<Iterator, void()> omit_strings_;
    qi::rule<Iterator, std::vector<std::string>()> commit_files_;
    qi::rule<Iterator, std::string()> commit_user_;
    qi::rule<Iterator, std::vector<std::string>()> commit_comments_;
    qi::rule<Iterator, logGrammarType()> result_;
};

}

parseSVNLog::parseSVNLog()
{
}

std::vector<svnLogStructure>
parseSVNLog::parse_logs(const std::string &file_name) const
{        
    auto const Content = read_whole_file(file_name);

    logGrammarType logs;
    logGrammar<decltype(std::begin(Content))> grammar;
    parse_log_data(std::begin(Content), std::end(Content), grammar, logs);

    for(auto &log : logs){
        auto const It = boost::remove_if(log.commit_files_, [](std::string const &data)
        {
           return data.size() < 5;
        });
        log.commit_files_.erase(It, std::end(log.commit_files_));
        boost::algorithm::trim(log.commit_user_);
    }

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
