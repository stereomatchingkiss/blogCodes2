#ifndef LINUX_INTERFACES_PARSER_H
#define LINUX_INTERFACES_PARSER_H

/**
 * @brief The parser of the /etc/network/interfaces
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
    struct log_info
    {
        std::string  address;
        std::string  network;
        std::string  netmask;
        std::string  broadcast;
        std::string  gateway;
        std::string  mtu;
    };

private:
    void split_to_interface_structure(std::vector<std::string> const &input);

private:
    std::map<std::string, std::vector<std::string> > interface_info_;
    boost::scoped_ptr<parser> parser_;
};

#endif // LINUX_INTERFACES_PARSER_H
