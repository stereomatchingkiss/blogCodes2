#include "simpleDelimiter.hpp"

#include <vector>

#include <boost/spirit/include/qi.hpp>

namespace spiritParser
{

namespace qi = boost::spirit::qi;

template<typename Iterator>
bool simple_tokenizer(Iterator begin, Iterator end, char const *delimiter, std::vector<std::string> &output)
{
    bool r = qi::parse(begin, end, *~qi::char_(delimiter) % delimiter, output);

    if(!r || begin != end){
        return false;
    }

    return r;
}

template<typename Iterator>
inline bool simple_tokenizer(Iterator begin, Iterator end, std::string const &delimiter, std::vector<std::string> &output)
{
    return simple_tokenizer(begin, end, delimiter.c_str(), output);
}

}

void test_simple_delimiter()
{
    std::string strs("123|456|789|123123|444");
    std::vector<std::string> output;
    std::cout<<spiritParser::simple_tokenizer(std::begin(strs), std::end(strs), "|", output)<<std::endl;
    for(auto const &str : output){
        std::cout<<str<<std::endl;
    }
}
