#ifndef ADDER_HPP
#define ADDER_HPP

#include <iostream>
#include <string>

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>

template <typename Iterator>
bool adder(Iterator first, Iterator last, double& n)
{
    using boost::spirit::qi::double_;
    using boost::spirit::qi::_1;
    using boost::spirit::qi::phrase_parse;
    using boost::spirit::ascii::space;
    using boost::phoenix::ref;

    bool r = phrase_parse(first, last,

        //  Begin grammar
        (
            double_[ref(n) = _1] >> *(',' >> double_[ref(n) += _1])
        )
        ,
        //  End grammar

        space);

    if (first != last) // fail if we did not get a full match
        return false;
    return r;
}

inline void test_adder()
{
    std::string const nums{"1,2,3,4,5,6,7,8,9,10"};
    double sum;
    adder(std::begin(nums), std::end(nums), sum);
    std::cout<<sum<<std::endl;
}

#endif // ADDER_HPP
