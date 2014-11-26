#include <iostream>

#include "parser.hpp"

void test_parser()
{
    parser p;
    std::string const IP = "(\\d{1,3}:\\d{1,3}:\\d{1,3}:\\d{1,3})";
    auto const Pattern = "\\s+netmask\\s+" + IP;
    auto const Vec = p.get_match_words(Pattern,
                                       "  netmask 244:345:556:666");
    for(auto const &Data : Vec){
        std::cout<<Data<<std::endl;
    }

    std::cout<<p.get_match_word(Pattern, "  netmask 244:345:556:666")<<std::endl;
}

int main()
{    
    test_parser();

    return 0;
}

