#include "parseSVNLog.hpp"

#include <boost/algorithm/string.hpp>

//#define FUSION_MAX_VECTOR_SIZE 11

#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/include/io.hpp>
#include <boost/fusion/adapted/std_pair.hpp> //without this, spirit can't accept the type with std::pair

#include <boost/range.hpp>
#include <boost/range/algorithm.hpp>

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
        std::cerr<<"parse fail at "<<(begin - OriginBegin)<<std::endl;
        //std::cout<<std::string(OriginBegin, begin)<<std::endl;
        //std::cout<<"-----------success?-------------------"<<std::endl;
        return false;
    }

    return true;
}

}

namespace{

using logGrammarType = std::vector<parseSVNLog::logStructure>;

template <typename Iterator>
struct logGrammar : qi::grammar<Iterator, logGrammarType()>
{
    logGrammar() : logGrammar::base_type(result_)
    {
        dash_ = *qi::eol>> qi::omit[*qi::char_("-")] >> +qi::eol;
        revision_ =  dash_ >> qi::omit["r"] >> qi::uint_ >> " | ";
        branch_ = *~qi::char_('|');
        commit_year_ = "| " >> qi::uint_;
        commit_month_ = "-" >> qi::uint_ >> "-";
        commit_day_ = qi::uint_ >> qi::blank;
        hh_mm_ss_ = qi::uint_ >> ":" >> qi::uint_ >> ":" >> qi::uint_;
        omit_strings_ = qi::repeat(2)[qi::omit[*~qi::char_('\n')] >> *qi::eol];
        commit_files_ = omit_strings_ >> *(qi::blank >> *~qi::char_('\n') >>
                                           qi::eol) >> *qi::eol;
        commit_user_ = (qi::omit[*qi::alnum] >> qi::blank
                                             >> (qi::omit[qi::uint_] >> -qi::omit[',']) % qi::blank
                >> *~qi::char_('\n') >> qi::eol) |
                *~qi::char_('\n') >> qi::eol;
        commit_comments_ = *(!qi::eol >> *~qi::char_("\n") >> qi::eol);

        result_ = *(revision_ >> branch_ >> commit_year_
                    >> commit_month_ >> commit_day_
                    >> hh_mm_ss_ >> commit_files_
                    >> commit_user_ >> commit_comments_);
    }

    qi::rule<Iterator, void()> dash_;
    qi::rule<Iterator, size_t()> revision_;
    qi::rule<Iterator, std::string()> branch_;
    qi::rule<Iterator, size_t()> commit_month_;
    qi::rule<Iterator, size_t()> commit_day_;
    qi::rule<Iterator, size_t()> commit_year_;
    qi::rule<Iterator, parseSVNLog::hh_mm_ss()> hh_mm_ss_;
    qi::rule<Iterator, void()> omit_strings_;
    qi::rule<Iterator, std::vector<std::string>()> commit_files_;
    qi::rule<Iterator, std::string()> commit_user_;
    qi::rule<Iterator, std::vector<std::string>()> commit_comments_;
    qi::rule<Iterator, logGrammarType()> result_;
};

}

BOOST_FUSION_ADAPT_STRUCT(
        parseSVNLog::hh_mm_ss,
        (size_t, hh_)
        (size_t, mm_)
        (size_t, ss_)
        )


BOOST_FUSION_ADAPT_STRUCT(
        parseSVNLog::logStructure,
        (size_t, revision_)
        (std::string, branch_)
        (size_t, commit_year_)
        (size_t, commit_month_)
        (size_t, commit_day_)
        (parseSVNLog::hh_mm_ss, hh_mm_ss_)
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
    auto const Content = read_whole_file(file_name);

    logGrammarType logs;
    logGrammar<std::string::const_iterator> grammar;
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

std::ostream &operator<<(std::ostream &out, const parseSVNLog::logStructure &log)
{    
    out<<log.revision_<<", "<<log.branch_<<std::endl;
    out<<log.commit_year_<<"-"<<log.commit_month_<<"-";
    out<<log.commit_day_<<", ";
    //out<<log.hh_mm_ss_[0]<<":"<<log.hh_mm_ss_[1];
    //out<<":"<<log.hh_mm_ss_[2]<<std::endl;
    out<<log.hh_mm_ss_.hh_<<":"<<log.hh_mm_ss_.mm_;
    out<<":"<<log.hh_mm_ss_.ss_<<std::endl;
    for(auto const &Str : log.commit_files_){
        out<<Str<<std::endl;
    }
    out<<log.commit_user_<<std::endl;

    for(auto const &Str : log.commit_comments_){
        out<<Str<<std::endl;
    }

    return out;
}


parseSVNLog::logStructure::logStructure() :
    revision_{0},
    commit_year_{1000},
    commit_month_{0},
    commit_day_{32}
{}
