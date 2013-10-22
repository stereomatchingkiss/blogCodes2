#ifndef EXERCISE00_HPP
#define EXERCISE00_HPP

#include <cmath>
#include <iostream>
#include <string>
#include <vector>

#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/include/io.hpp>

#include <boost/spirit/include/karma.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>

namespace spiritParser
{

namespace karma = boost::spirit::karma;
namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;

struct videoData
{
    float time;
    std::string content;
};

struct transformData
{
    size_t index;
    std::vector<std::vector<int>> nums;
};

}

BOOST_FUSION_ADAPT_STRUCT(
        spiritParser::videoData,
        (float, time)
        (std::string, content)
        )

BOOST_FUSION_ADAPT_STRUCT(
        spiritParser::transformData,
        (size_t, index)
        (std::vector<std::vector<int>>, nums)
        )

namespace spiritParser
{

    template<typename Iterator>
    bool parse_video_data(Iterator begin, Iterator end, std::vector<videoData> &data)
    {
        bool r = qi::parse(begin, end, *(qi::float_ >> qi::skip["\">"] >> *~qi::char_('\n') >> *(qi::char_("\n"))), data);

        if(!r || begin != end){
            return false;
        }

        return r;
    }

    template<typename OutputIterator>
    struct videoGrammar : karma::grammar<OutputIterator, transformData()>
    {
        videoGrammar()
            : videoGrammar::base_type(final_rule)
        {
            using karma::int_;
            using karma::repeat;
            using karma::right_align;
            using karma::uint_;

            first_rule = repeat(2)[right_align(2, '0')[int_ <<":"]]<<
                         right_align(2, '0')[int_] <<","<< right_align(2, '0')[int_];

            second_rule = first_rule << " --> " << first_rule << "\n";
            final_rule = uint_ << "\n"<< second_rule;

        }

        karma::rule<OutputIterator, transformData()> final_rule;
        karma::rule<OutputIterator, std::vector<std::vector<int>>()> second_rule;
        karma::rule<OutputIterator, std::vector<int>()> first_rule;
    };

    void generate_nums(std::vector<int> &nums, float data)
    {
        nums[0] = (int)data/3600;
        nums[1] = (int)std::floor(data)/60%60;
        nums[2] = (int)std::floor(data)%60;
        nums[3] = (int)((data-std::floor(data))*1000.0);
    }

    /**
 *  fir = 0.5, sec = 5.19
 *  transform to : 00:00:00,500 --> 00:00:05,190
 */
    template<typename Iterator, typename Grammar>
    bool generate_times(Iterator sink, Grammar &grammar, transformData &data, size_t index, float fir, float sec)
    {

        using karma::int_;
        using karma::uint_;
        using karma::right_align;

        data.index = index;
        generate_nums(data.nums[0], fir);
        generate_nums(data.nums[1], sec);
        bool r = karma::generate(sink,
                                 grammar,
                                 data
                                 );

        return r;
    }

/**

Original :

0.5">If you are a premiere member of the lynda.com online training library, or if you

5.19">are watching this tutorial on a DVD, you have access to the exercise files used throughout the title.

After transform :

1
00:00:00,500 --> 00:00:05,190
If you are a premiere member of the lynda.com online training library, or if you

2
00:00:05,190 --> 00:00:11,800
are watching this tutorial on a DVD, you have access to the exercise files used throughout the title.
*/
void exercise_00()
{
    std::string strs{"0.5\">If you are a premiere member of the lynda.com online training library, or if you\n\n"
                     "5.19\">are watching this tutorial on a DVD, you have access to the exercise files used throughout the title.\n\n"
                     "11.8\">are watching this tutorial on a DVD, you have access to the exercise files used throughout the title.\n\n"};

    std::vector<spiritParser::videoData> video;
    spiritParser::parse_video_data(std::begin(strs), std::end(strs), video);

    if(video.size() < 2) {
        std::cerr<<"not enough of data"<<std::endl;
        return;
    }

    std::string number;
    std::string result{std::move(strs)};
    result.clear();
    std::back_insert_iterator<std::string> sink(number);
    size_t const Size = video.size() - 1;
    //use karma to generate the time label is a little bit overkill
    //since the length of the time label are known, sprintf or stringstream
    //should be able to do the job(even it may slower than spirit).
    //However, for practice, I chose spirit::karma to generate the time label
    spiritParser::videoGrammar<std::back_insert_iterator<std::string>> grammar;
    spiritParser::transformData data{0, std::vector<std::vector<int>>(2, std::vector<int>(4))};
    for(size_t i = 0; i != Size; ++i){
        spiritParser::generate_times(sink, grammar, data, i + 1, video[i].time, video[i + 1].time);
        result += number;
        result += video[i].content;
        number.clear();
    }

    std::cout<<result<<std::endl;
}

}

inline void test_exercise_00()
{
    spiritParser::exercise_00();
}

#endif // EXERCISE00_HPP
