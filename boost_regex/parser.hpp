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

    template<typename OutputIter>
    std::string get_match_word(std::string const &pattern,
                               OutputIter begin,
                               OutputIter end) const;

private:
};

/**
 *@brief match the word(only the first word will be match) of pattern\n
 * specify
 *
 *@param pattern regular expression(perl style)
 *@param begin begin position
 *@param end past the end position
 */
template<typename OutputIter>
std::string parser::get_match_word(const std::string &pattern,
                                   OutputIter begin,
                                   OutputIter end) const
{
    for(OutputIter it = begin; it != end; ++it){
        std::string result = get_match_word(pattern, *it);
        if(!result.empty()){
            return result;
        }
    }

    return "";
}

#endif // FILE_PARSER_HPP
