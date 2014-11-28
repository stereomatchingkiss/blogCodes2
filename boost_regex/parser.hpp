#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
#include <vector>

#include <iostream>

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

    template<typename OutIter>
    StrVec get_match_words(std::string const &pattern,
                           OutIter begin,
                           OutIter end) const;

    template<typename OutIter>
    std::vector<StrVec> get_many_match_words(std::string const &pattern,
                                             OutIter begin,
                                             OutIter end) const;

    std::string get_match_word(std::string const &pattern,
                               std::string const &input) const;

    std::string get_match_word(std::string const &pattern,
                               StrVec const &input) const;

    template<typename OutIter>
    std::string get_match_word(std::string const &pattern,
                               OutIter begin,
                               OutIter end) const;

private:
};

/**
 *@brief match the word of pattern specify.\n
 * This function will try to match the pattern of\n
 * every sentence and store the match result into\n
 * a vector<vector<string>>
 *
 *@param pattern regular expression(perl style)
 *@param begin begin position(it could be a sentence, paragraph etc)
 *@param end past the end position
 */
template<typename OutIter>
std::vector<parser::StrVec> parser::get_many_match_words(const std::string &pattern,
                                                         OutIter begin,
                                                         OutIter end) const
{
    std::vector<StrVec> result;
    for(OutIter it = begin; it != end; ++it){
        StrVec temp = get_match_words(pattern, *it);
        if(!temp.empty()){
            //in c++11, could use move to reduce one copy
            result.push_back(temp);
        }
    }

    return result;
}

/**
 *@brief match the word of pattern specify.\n
 * This function can match multiple words, but will\n
 * only catch the words from a line(every OutputIterator)\n
 * point to a line(a bunch of characters)
 *
 *@param pattern regular expression(perl style)
 *@param begin begin position(it could be a sentence, paragraph etc)
 *@param end past the end position
 */
template<typename OutIter>
parser::StrVec parser::get_match_words(const std::string &pattern,
                                       OutIter begin,
                                       OutIter end) const
{
    for(OutIter it = begin; it != end; ++it){
        StrVec result = get_match_words(pattern, *it);
        std::cout<<*it<<std::endl;
        if(!result.empty()){
            return result;
        }
    }

    return StrVec();
}

/**
 *@brief match the word(only the first word will be match) of pattern\n
 * specify
 *
 *@param pattern regular expression(perl style)
 *@param begin begin position(it could be a sentence, paragraph etc)
 *@param end past the end position
 */
template<typename OutIter>
std::string parser::get_match_word(const std::string &pattern,
                                   OutIter begin,
                                   OutIter end) const
{
    for(OutIter it = begin; it != end; ++it){
        std::string result = get_match_word(pattern, *it);
        if(!result.empty()){
            return result;
        }
    }

    return "";
}

#endif // FILE_PARSER_HPP
