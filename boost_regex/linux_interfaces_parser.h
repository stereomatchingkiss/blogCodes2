#ifndef LINUX_INTERFACES_PARSER_H
#define LINUX_INTERFACES_PARSER_H

/**
 * @brief The parser of the /etc/network/interfaces\n
 * not so efficient, but should adequate for the small project\n
 *
 *
 * This class could eliminate some useless copy if\n
 * the compiler support c++11
 */

#include <boost/scoped_ptr.hpp>

#include <map>
#include <string>
#include <vector>

class parser;

class linux_interfaces_parser
{
public:
    linux_interfaces_parser(std::string const &input = "");
    ~linux_interfaces_parser();

    std::string get_broadcast(std::string const &interface_card) const;
    std::string get_netmask(std::string const &interface_card) const;
    std::vector<std::string> get_routers(std::string const &interface_card) const;

    void set_contents(std::string const &input);
    void set_contents(std::vector<std::string> const &input);    
private:
    struct router_infomation
    {
        std::string gateway;
        std::string mask;
        std::string route;
    };

    struct log_info
    {
        std::string  address;
        std::string  broadcast;
        std::string  gateway;
        std::string  mtu;
        std::string  netmask;
        std::string  network;                
        std::vector<router_infomation>  routers;
    };

private:
    typedef std::vector<std::string>::const_iterator CIter;
    typedef std::vector<router_infomation> RouteInfo;

    void build_interface_info(std::vector<std::string> const &input,
                              std::vector<std::pair<size_t, std::string> > const &eth_info);

    RouteInfo match_router(CIter begin, CIter end) const;

    void set_interface_info(std::string const &interface, CIter begin, CIter end);
    void split_to_interface_structure(std::vector<std::string> const &input);

private:
    std::map<std::string, log_info> interface_info_;
    std::string const IPPattern_;
    boost::scoped_ptr<parser> parser_;
};

#endif // LINUX_INTERFACES_PARSER_H
