#ifndef PARSECOMPLEXNUM_HPP
#define PARSECOMPLEXNUM_HPP

#include <complex>
#include <iostream>
#include <string>

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
//#include <boost/spirit/include/support_multi_pass.hpp>
//#include <boost/spirit/include/classic_position_iterator.hpp>

template <typename Iterator>
bool parse_complex(Iterator first, Iterator last, std::complex<size_t>& c)
{
    using boost::spirit::qi::char_;
    using boost::spirit::qi::hex;
    using boost::spirit::qi::_1;
    using boost::spirit::qi::phrase_parse;
    using boost::spirit::ascii::space;
    using boost::phoenix::ref;

    double rN = 0.0;
    double iN = 0.0;
    bool r = phrase_parse(first, last,

        //  Begin grammar
        (
                '(' >> hex[ref(rN) = _1]
            >> -(+char_(',') >> hex[ref(iN) = _1]) >> ')'
        ),
        //  End grammar

        space);

    if (!r || first != last) // fail if we did not get a full match
        return false;
    c = std::complex<double>(rN, iN);
    return r;
}

inline void test_parse_complex_num()
{
    std::string const nums[] = {"(BA,,,,88899)", "444", "(0,1)", "(89,,223)"};
    for(auto const &num : nums){
        std::complex<size_t> c;
        parse_complex(std::begin(num), std::end(num), c);
        std::cout<<c<<std::endl;
        std::cout<<num<<std::endl;
    }
}

#endif // PARSECOMPLEXNUM_HPP
