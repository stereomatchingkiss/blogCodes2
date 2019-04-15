#ifndef CONFIG_PARSER_HPP
#define CONFIG_PARSER_HPP

#include <string>

struct bg_subtractor_config;

bg_subtractor_config parse_bg_subtractor_config(std::string const &filename);

#endif // CONFIG_PARSER_HPP
