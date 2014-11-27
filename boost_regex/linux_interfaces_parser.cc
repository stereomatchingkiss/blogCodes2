#include "parser.hpp"
#include "linux_interfaces_parser.h"

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>

#include <boost/regex.hpp>

#include <iostream>

linux_interfaces_parser::linux_interfaces_parser(std::string const &input) :
parser_(new parser)
{
    set_contents(input);
}

linux_interfaces_parser::~linux_interfaces_parser()
{

}

void linux_interfaces_parser::set_contents(std::string const &input)
{
    std::vector<std::string> lines;
    boost::split(lines, input, boost::is_any_of("\n"));
    split_to_interface_structure(lines);
}

void linux_interfaces_parser::set_contents(std::vector<std::string> const &input)
{
    split_to_interface_structure(input);
}

void linux_interfaces_parser::split_to_interface_structure(const std::vector<std::string> &input)
{
    interface_info_.clear();

    std::vector<std::pair<size_t, std::string> > eth_info;
    for(size_t i = 0; i != input.size(); ++i){
        //captured eth from context like
        //"iface eth0 inet static"
        std::string const Captured =
                parser_->get_match_word("\\s*iface\\s+(eth[01]).*", input[i]);
        if(!Captured.empty()){
            eth_info.push_back(std::make_pair(i, Captured));
            std::cout<<i<<" : "<<input[i]<<", ";
            std::cout<<"captured : "<<Captured<<std::endl;
        }
    }

    for(size_t i = 0; i != eth_info.size(); ++i){
        for(size_t j = 0; j != input.size(); ++j){

        }
    }
}
