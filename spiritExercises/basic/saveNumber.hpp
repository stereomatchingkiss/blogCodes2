#ifndef SAVENUMBER_HPP
#define SAVENUMBER_HPP

#include <iostream>
#include <string>
#include <vector>

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>

template <typename Iterator>
bool save_numbers(Iterator first, Iterator last, std::vector<double>& v)
{
    using boost::spirit::qi::char_;
    using boost::spirit::qi::double_;
    using boost::spirit::qi::_1;
    using boost::spirit::qi::phrase_parse;
    using boost::spirit::ascii::space;
    using boost::phoenix::push_back;

    /*bool r = phrase_parse(first, last,

                          //  Begin grammar
                          (
                              //double_[push_back(boost::phoenix::ref(v), _1)]
                              //    >> *(',' >> double_[push_back(boost::phoenix::ref(v), _1)])
                              double_[push_back(boost::phoenix::ref(v), _1)] % ','
                          )
            ,
            //  End grammar

            space);*/

    bool r = phrase_parse(first, last,

                          //  Begin grammar
                          (
                              double_ % ','
                          )
            ,
            //  End grammar
            space, v);

    if (first != last) // fail if we did not get a full match
        return false;
    return r;
}

inline void test_save_numbers()
{
    std::string const nums{"1,2,3,4,5,6,7,8,9,10"};
    std::vector<double> v;
    save_numbers(std::begin(nums), std::end(nums), v);
    for(auto data : v){
        std::cout<<data<<", ";
    }
    std::cout<<std::endl;
}

#endif // SAVENUMBER_HPP
