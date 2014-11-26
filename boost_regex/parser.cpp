#include "parser.hpp"

#include <boost/regex.hpp>

parser::parser()
{
}

/**
 * @brief get the match words
 * @param pattern the pattern want to match
 * @param input input string
 * @return match results, empty if nothing matched
 */
std::vector<std::string> parser::
get_match_words(const std::string &pattern,
                std::string const &input) const
{
    boost::regex const Reg(pattern);
    boost::smatch match;
    std::vector<std::string> result;
    if (boost::regex_match(input, match, Reg)) {
        if (match.size() >= 2) {
            for(size_t i = 1; i != match.size(); ++i){
                result.push_back(match[i]);
            }
        }
    }

    return result;
}

/**
 * @brief get the match word
 * @param pattern the pattern want to match
 * @param input input string
 * @return match result, empty if nothing matched
 */
std::string parser::get_match_word(const std::string &pattern,
                                   const std::string &input) const
{
    boost::regex const Reg(pattern);
    boost::smatch match;
    if (boost::regex_match(input, match, Reg)) {
        if (match.size() >= 2) {
            return match[1];
        }
    }

    return "";
}

/**
 * @brief get the match word
 * @param pattern the pattern want to match
 * @param input input string
 * @return match result, empty if nothing matched
 */
std::string parser::get_match_word(const std::string &pattern,
                                   const parser::StrVec &input) const
{
    std::string result;
    for(size_t i = 0; i != input.size(); ++i){
        result = get_match_word(pattern, input[i]);
        if(!result.empty()){
            break;
        }
    }

    return result;
}
