#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
#include <vector>

/**
 * @brief A small parser to parse some string\n
 * based on the regular expression(Perl like Regex)
 */

class parser
{
public:
    using StrVec = std::vector<std::string>;

    parser();

    StrVec get_match_words(std::string const &pattern,
                           std::string const &input) const;

    std::string get_match_word(std::string const &pattern,
                               std::string const &input) const;

    std::string get_match_word(std::string const &pattern,
                               StrVec const &input) const;

private:
};

#endif // FILE_PARSER_HPP
