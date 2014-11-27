#include "parser.hpp"
#include "linux_interfaces_parser.h"

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>

#include <boost/regex.hpp>

#include <iostream>

linux_interfaces_parser::linux_interfaces_parser(std::string const &input) :
    IPPattern_("(\\d{1,3}.\\d{1,3}.\\d{1,3}.\\d{1,3})\\s*"),
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

void linux_interfaces_parser::set_interface_info(const std::string &interface,
                                                 CIter begin,
                                                 CIter end)
{
    interface_info_.insert(std::make_pair(interface, log_info()));
    log_info &info = interface_info_[interface];

    info.address = parser_->get_match_word("\\s+address\\s+" + IPPattern_, begin, end);
    std::cout<<info.address<<std::endl;

    info.broadcast = parser_->get_match_word("\\s+broadcast\\s+" + IPPattern_, begin, end);
    std::cout<<info.broadcast<<std::endl;

    info.gateway = parser_->get_match_word("\\s+gateway\\s+" + IPPattern_, begin, end);
    std::cout<<info.gateway<<std::endl;

    info.mtu = parser_->get_match_word("\\s+mtu\\s+(\\d+)", begin, end);
    std::cout<<info.mtu<<std::endl;

    info.network = parser_->get_match_word("\\s+network\\s+" + IPPattern_, begin, end);
    std::cout<<info.network<<std::endl;

    info.netmask = parser_->get_match_word("\\s+netmask\\s+" + IPPattern_, begin, end);
    std::cout<<info.netmask<<std::endl;
}

void linux_interfaces_parser::build_interface_info(std::vector<std::string> const &input,
                                                   std::vector<std::pair<size_t, std::string> > const &eth_info)
{
    if(!eth_info.empty()){
        interface_info_.clear();
        if(eth_info.size() == 1){
            //std::cout<<"eth info size == 1"<<std::endl;
            set_interface_info(eth_info[0].second,
                    input.begin() + eth_info[0].first,
                    input.end());
        }else if(eth_info.size() == 2){
            //std::cout<<"eth info size == 2"<<std::endl;
            set_interface_info(eth_info[0].second,
                    input.begin() + eth_info[0].first,
                    input.begin() + eth_info[1].first);
            //std::cout<<"eth 0 setup"<<std::endl;

            set_interface_info(eth_info[1].second,
                    input.begin() + eth_info[1].first,
                    input.end());
            //std::cout<<"eth 1 setup"<<std::endl;
        }
    }
}

/**
 * @brief split the information of eth0 and eth1 into diffrent structure
 * @param input information of interfaces
 */
void linux_interfaces_parser::split_to_interface_structure(const std::vector<std::string> &input)
{    
    std::vector<std::pair<size_t, std::string> > eth_info;
    for(size_t i = 0; i != input.size(); ++i){
        //captured eth from context like
        //"iface eth0 inet static"
        std::string const Captured =
                parser_->get_match_word("\\s*iface\\s+(eth[01]).*", input[i]);
        if(!Captured.empty()){
            eth_info.push_back(std::make_pair(i, Captured));
            //std::cout<<i<<" : "<<input[i]<<", ";
            //std::cout<<"captured : "<<Captured<<std::endl;
        }
    }

    build_interface_info(input, eth_info);
}
