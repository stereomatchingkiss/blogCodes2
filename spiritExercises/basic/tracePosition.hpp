#ifndef TRACEPOSITION_HPP
#define TRACEPOSITION_HPP

#include <iostream>
#include <string>

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>

#include <boost/spirit/include/support_line_pos_iterator.hpp>

namespace client
{
    template <typename Iterator>
    bool trace_numbers(Iterator first, Iterator last)
    {
        using boost::spirit::qi::char_;
        using boost::spirit::qi::double_;
        using boost::spirit::qi::_1;
        using boost::spirit::qi::phrase_parse;
        using boost::spirit::ascii::space;
        using boost::phoenix::push_back;

        bool r = phrase_parse(first, last,

                              //  Begin grammar
                              (
                                  double_ % ','
                              )
                ,
                //  End grammar
                space);

        if (first != last) // fail if we did not get a full match
            return false;
        return r;
    }
}

inline void test_trace_position()
{
    //std::string const nums{"1,2,3,4,5,6,7,8,9,10"};
    //boost::spirit::line_pos_iterator<decltype(nums.begin())> begin(nums.begin()), end(nums.end());
    //client::trace_numbers(begin, end);
}

#endif // TRACEPOSITION_HPP
