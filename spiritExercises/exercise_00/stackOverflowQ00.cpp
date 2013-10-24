#include <cstring>
#include <cstdio>

#include <functional>
#include <iostream>
#include <iterator>
#include <vector>

#include <timeEstimate.hpp>

#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/include/io.hpp>

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_auto.hpp>

#include "stackOverflowQ00.hpp"

/****************************strtok_r************************/

namespace
{

struct sTokenDataC {
    char *time;
    char *symb;
    float bid;
    float ask;
    int bidSize;
    int askSize;
};

sTokenDataC parseTick( char *line, char *parseBuffer )
{
    sTokenDataC tokenDataOut;

    tokenDataOut.time = strtok_r( line,",", &parseBuffer );
    tokenDataOut.symb = strtok_r( nullptr,",", &parseBuffer );
    tokenDataOut.bid = atof(strtok_r( nullptr,",", &parseBuffer ));
    tokenDataOut.ask = atof(strtok_r( nullptr , ",", &parseBuffer ));
    tokenDataOut.bidSize = atoi(strtok_r( nullptr,",", &parseBuffer ));
    tokenDataOut.askSize = atoi(strtok_r( nullptr, ",", &parseBuffer  ));

    return tokenDataOut;
}

namespace qi = boost::spirit::qi;

struct tokenDataCPP
{
    std::string time;
    std::string symb;
    float bid;
    float ask;
    int bidSize;
    int askSize;

    void clearTimeSymb(){
        time.clear(); symb.clear();
    }
};

// A function object
struct positionMarker
{
    positionMarker() : position(0) {}

    void get_pos(std::vector<char> const &i)
    {
        position = i.size();
    }

    size_t position;
};

}

BOOST_FUSION_ADAPT_STRUCT(
        tokenDataCPP,
        (std::string, time)
        (std::string, symb)
        (float, bid)
        (float, ask)
        (int, bidSize)
        (int, askSize)
        )

void test_strcpy_s(int iteration)
{
    std::vector<char> testStringC(64);
    std::vector<char> lineBuffer(64);

    printf("test_strcpy_s....\n");
    strcpy(&testStringC[0],"09:30:00,TEST,13.24,15.32,10,14");
    {
        timeEstimate<> es;
        sTokenDataC tokenData2;
        for(int i = 0; i != iteration; ++i)
        {
            strcpy(&lineBuffer[0], &testStringC[0]);//this is more realistic since this has to happen because I want to preserve the line
            tokenData2 = parseTick(&lineBuffer[0], &testStringC[0]);
            //std::cout<<*tokenData2.time<<", "<<*tokenData2.symb<<",";
            //printf("%s, %s, ", tokenData2.time, tokenData2.symb);
            //std::cout<<tokenData2.bid<<", "<<tokenData2.ask<<", "<<tokenData2.bidSize<<", "<<tokenData2.askSize<<std::endl;
        }
    }
}

void test_spirit_qi(int iteration)
{
    std::string const strs{"09:30:00,TEST,13.24,15.32,10,14"};
    tokenDataCPP data;

    auto const myString = *~qi::char_(",");
    auto const parser = myString >> "," >> myString >> ","
                        >> qi::float_ >> "," >> qi::float_ >> "," >> qi::int_  >> "," >> qi::int_;

    std::cout<<("test_spirit_qi....\n");
    {
        timeEstimate<> es;
        for(int i = 0; i != iteration; ++i){
            qi::parse(std::begin(strs), std::end(strs), parser, data);
            //std::cout<<strs.substr(0, pos_1.position)<<","<<strs.substr(pos_1.position + 1, pos_2.position)<<",";
            //std::cout<<data.time<<","<<data.symb<<",";
            //std::cout<<data.bid<<","<<data.ask<<","<<data.bidSize<<","<<data.askSize<<std::endl;
            data.clearTimeSymb();
        }
    }
}

void test_so_00()
{
    int const ITERATIONS = 500 * 10000;
    test_strcpy_s(ITERATIONS);
    test_spirit_qi(ITERATIONS);
}


