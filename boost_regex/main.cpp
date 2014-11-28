#include <iostream>

#include "linux_interfaces_parser.h"
#include "parser.hpp"

std::string const Input = "auto lo\n"
                          "iface lo inet loopback\n"
                          "auto eth0\n"
                          "iface eth0 inet static\n"
                          "    address 192.168.82.163\n"
                          "    network 192.168.82.0\n"
                          "    netmask 255.255.255.0\n"
                          "    broadcast 192.168.82.255\n"
                          "    gateway 192.168.82.163\n"
                          "    mtu 1500\n"
                          "post-up route add -net 192.168.88.255 netmask 255.255.255.0 gw 192.168.88.1\n"
                          "post-up route add -net 192.168.88.33 netmask 255.155.255.0 gw 192.999.88.1\n"
                          "pre-down route add -net 192.168.88.255 netmask 255.255.255.0 gw 192.168.88.2\n"
                          "iface eth1 inet dhcp\n";

void test_linux_parser()
{
    linux_interfaces_parser parser(Input);
}

void test_parser()
{
    parser p;
    std::string const IP = "(\\d{1,3}:\\d{1,3}:\\d{1,3}:\\d{1,3})\\s*";
    auto const Pattern = "[^0-9]+" + IP;
    auto const Vec = p.get_match_words(Pattern,
                                       "  address 244:345:556:666    ");
    for(auto const &Data : Vec){
        std::cout<<Data<<std::endl;
    }

    std::cout<<p.get_match_word(Pattern, "  address 244:345:556:666")<<std::endl;

    std::string const RoutePrefix = "[^0-9]+";
    std::string const IP2 = "(\\d{1,3}.\\d{1,3}.\\d{1,3}.\\d{1,3})\\s+";
    std::string const Router = RoutePrefix + IP2;
    //std::string const Netmask = "netmask\\s+" + IP2;
    //std::string const GateWay = "gw\\s+" + IP2;
    std::cout<<p.get_match_word(Router,
                                "post-up route add -net 192.168.88.255  ")
            <<std::endl;
}

int main()
{    
    test_linux_parser();
    //test_parser();

    return 0;
}

