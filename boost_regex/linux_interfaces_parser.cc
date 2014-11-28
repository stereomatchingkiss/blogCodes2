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
    info.broadcast = parser_->get_match_word("\\s+broadcast\\s+" + IPPattern_, begin, end);
    info.gateway = parser_->get_match_word("\\s+gateway\\s+" + IPPattern_, begin, end);
    info.mtu = parser_->get_match_word("\\s+mtu\\s+(\\d+)", begin, end);
    info.network = parser_->get_match_word("\\s+network\\s+" + IPPattern_, begin, end);
    info.netmask = parser_->get_match_word("\\s+netmask\\s+" + IPPattern_, begin, end);

    info.routers = match_router(begin, end);
}

/**
 * @brief setup the info of the interfaces
 * @param input contents of the interfaces
 * @param eth_info indicate the information belongs\n
 * to eth0 or eth1
 */
void linux_interfaces_parser::build_interface_info(std::vector<std::string> const &input,
                                                   std::vector<std::pair<size_t, std::string> > const &eth_info)
{
    if(!eth_info.empty()){
        interface_info_.clear();
        if(eth_info.size() == 1){
            set_interface_info(eth_info[0].second,
                    input.begin() + eth_info[0].first,
                    input.end());
        }else if(eth_info.size() == 2){
            set_interface_info(eth_info[0].second,
                    input.begin() + eth_info[0].first,
                    input.begin() + eth_info[1].first);

            set_interface_info(eth_info[1].second,
                    input.begin() + eth_info[1].first,
                    input.end());
        }
    }
}

/**
 * @brief parse the router information similar to\n
 * "post-up route add -net 192.168.88.255 netmask 255.255.255.0 gw 192.168.88.1"\n
 * "pre-down route add -net 192.168.88.255 netmask 255.255.255.0 gw 192.168.88.2"\n
 * @param begin begin position
 * @param end past the end position
 * @return router information of the router
 */
linux_interfaces_parser::RouteInfo
linux_interfaces_parser::match_router(CIter begin, CIter end) const
{
    std::string const RoutePrefix = "[^0-9]+";
    std::string const IP = "(\\d{1,3}.\\d{1,3}.\\d{1,3}.\\d{1,3})\\s*";
    std::string const Router = RoutePrefix + IP;
    std::string const Netmask = "netmask\\s+" + IP;
    std::string const GateWay = "gw\\s+" + IP;
    std::vector<std::vector<std::string> > result = parser_->get_many_match_words(
                Router + Netmask + GateWay,
                begin, end);

    RouteInfo info;
    for(size_t i = 0; i != result.size(); ++i){
        if(result[i].size() == 3){
            info.push_back(router_infomation());
            router_infomation &rif = info.back();
            std::vector<std::string> &match = result[i];
            rif.route.swap(match[0]);
            rif.mask.swap(match[1]);
            rif.gateway.swap(match[2]);
            std::cout<<"router match : "<<std::endl;
            std::cout<<"router == : "<<rif.route<<std::endl;
            std::cout<<"mask == : "<<rif.mask<<std::endl;
            std::cout<<"gateway == :"<<rif.gateway<<std::endl;
        }
    }

    return info;
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
        }
    }

    build_interface_info(input, eth_info);
}
