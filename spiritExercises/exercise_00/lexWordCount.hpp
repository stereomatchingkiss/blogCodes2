#ifndef LEXWORDCOUNT_HPP
#define LEXWORDCOUNT_HPP

#define BOOST_VARIANT_MINIMIZE_SIZE

#include <boost/config/warning_disable.hpp>
//[wcp_includes
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/lex_lexertl.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_statement.hpp>
#include <boost/spirit/include/phoenix_container.hpp>
//]

#include <iostream>
#include <string>
#include <vector>

namespace spiritParser
{

//[wcp_namespaces
using namespace boost::spirit;
using namespace boost::spirit::ascii;

//[wcp_token_ids
enum tokenids
{
    IDANY = lex::min_token_id + 10
};
//]

//[wcp_token_definition
template <typename Lexer>
struct number_position_track_tokens : lex::lexer<Lexer>
{
    number_position_track_tokens()
    {
        // define patterns (lexer macros) to be used during token definition
        // below
        this->self.add_pattern
            ("NUM", "[0-9]+")
        ;

        // define tokens and associate them with the lexer
        number = "{NUM}";    // reference the pattern 'NUM' as defined above

        // this lexer will recognize 3 token types: words, newlines, and
        // everything else
        this->self.add
            (number)          // no token id is needed here
            (".", IDANY)      // characters are usable as tokens as well
        ;
    }

    // the token 'number' exposes the matched string as its parser attribute
    lex::token_def<std::string> number;
};
//]

template<typename Iterator>
struct numberGrammar : qi::grammar<Iterator>
{
    template <typename TokenDef>
    numberGrammar(TokenDef const &tok)
      : numberGrammar::base_type(start)
      , num(0), position(0)
    {
        using boost::phoenix::ref;
        using boost::phoenix::push_back;
        using boost::phoenix::size;
        //"34, 44, 55, 66, 77, 88"
        start =  *(   tok.number        [++ref(num),
                                         boost::phoenix::push_back(boost::phoenix::ref(numPosition), boost::phoenix::ref(position)),
                                         ref(position) += size(_1)
                                        ]
                  |   qi::token(IDANY)  [++ref(position)]
                  )
              ;
    }

    std::size_t num, position;
    std::vector<size_t> numPosition;
    qi::rule<Iterator> start;
};

void lex_word_count_1();

}

inline void test_lex_word_count_1()
{
    spiritParser::lex_word_count_1();
}

#endif // LEXWORDCOUNT_HPP
