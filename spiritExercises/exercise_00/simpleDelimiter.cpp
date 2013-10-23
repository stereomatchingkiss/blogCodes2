#include "simpleDelimiter.hpp"

void test_simple_delimiter()
{
    std::string strs("123|456|789|123123|444");
    std::vector<std::string> output;
    std::cout<<spiritParser::simple_tokenizer(std::begin(strs), std::end(strs), "|", output)<<std::endl;
    for(auto const &str : output){
        std::cout<<str<<std::endl;
    }
}
