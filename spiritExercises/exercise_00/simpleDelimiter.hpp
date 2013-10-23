#ifndef SIMPLEDELIMITER_HPP
#define SIMPLEDELIMITER_HPP

#include <vector>

#include <boost/spirit/include/qi.hpp>

namespace spiritParser
{

namespace qi = boost::spirit::qi;

template<typename Iterator, typename T>
bool simple_tokenizer(Iterator begin, Iterator end, char const *delimiter, std::vector<T> &output)
{
    bool r = qi::parse(begin, end, *~qi::char_(delimiter) % delimiter, output);

    if(!r || begin != end){
        return false;
    }

    return r;
}

template<typename Iterator, typename T>
inline bool simple_tokenizer(Iterator begin, Iterator end, std::string const &delimiter, std::vector<T> &output)
{
    return simple_tokenizer(begin, end, delimiter.c_str(), output);
}

}

void test_simple_delimiter();

#endif // SIMPLEDELIMITER_HPP
