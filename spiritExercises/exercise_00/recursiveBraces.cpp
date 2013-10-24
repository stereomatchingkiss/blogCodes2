#include "recursiveBraces.hpp"

#include <iostream>
#include <string>

#include <boost/spirit/include/qi.hpp>

namespace spiritParser
{

namespace qi = boost::spirit::qi;

template<typename Iterator>
struct recursiveBraces : qi::grammar<Iterator, std::string(), qi::ascii::space_type>
{
    recursiveBraces() : recursiveBraces::base_type(expression)
    {
        braces = "(" >> *expression >> ")";
        expression = +braces;
        finalRules = +expression;
    }

    qi::rule<Iterator, std::string(), qi::ascii::space_type> braces;
    qi::rule<Iterator, std::string(), qi::ascii::space_type> expression;
    qi::rule<Iterator, std::string(), qi::ascii::space_type> finalRules;
};

inline void parse_recursive_braces()
{
    std::string const braces[] =
    {
        {"( () )"}, {"("}, {"(() ())"}, {"(  ( ( ( () ) ) ) )"},
        {"(((( ( ))))"}, {"(() ()) ((())) (())"}, {"(() ()) ((( ( ))) (())"}
    };
    recursiveBraces<std::string::const_iterator> grammar;

    for(auto const &data : braces){

        auto begin = std::begin(data), end = std::end(data);
        bool r = qi::phrase_parse(begin, end, grammar, qi::ascii::space);

        if(r && begin == end){
            std::cout<<"match"<<std::endl;
        }else{
            std::cout<<"wrong"<<std::endl;
        }
    }
}

}

void test_parse_recursive_braces()
{
    spiritParser::parse_recursive_braces();
}
