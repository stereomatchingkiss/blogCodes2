#include "lexWordCount.hpp"

namespace spiritParser
{

//[wcp_main
void lex_word_count_1()
{
/*<  Define the token type to be used: `std::string` is available as the
     type of the token attribute
>*/  using token_type = lex::lexertl::token<char const*, boost::mpl::vector<std::string> >;

/*<  Define the lexer type to use implementing the state machine
>*/  using lexer_type = lex::lexertl::lexer<token_type>;

/*<  Define the iterator type exposed by the lexer type
>*/  using iterator_type = number_position_track_tokens<lexer_type>::iterator_type;

    // now we use the types defined above to create the lexer and grammar
    // object instances needed to invoke the parsing process
    number_position_track_tokens<lexer_type> word_count;          // Our lexer
    numberGrammar<iterator_type> g (word_count);  // Our parser

    // read in the file int memory
    std::string str ("34, 44, 55, 66, 77, 88");
    char const* first = str.c_str();
    char const* last = &first[str.size()];

/*<  Parsing is done based on the the token stream, not the character
     stream read from the input. The function `tokenize_and_parse()` wraps
     the passed iterator range `[first, last)` by the lexical analyzer and
     uses its exposed iterators to parse the toke stream.
>*/  bool r = lex::tokenize_and_parse(first, last, word_count, g);

    if (r) {
        std::cout << "nums: " << g.num << ", positions: " << g.position <<std::endl;
        for(auto data : g.numPosition){
            std::cout<<"position : "<<data<<std::endl;
        }
    }
    else {
        std::string rest(first, last);
        std::cerr << "Parsing failed\n" << "stopped at: \""
                  << rest << "\"\n";
    }
}

}
