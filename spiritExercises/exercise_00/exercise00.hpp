#ifndef EXERCISE00_HPP
#define EXERCISE00_HPP

#include <cmath>
#include <iostream>
#include <string>
#include <vector>

#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/include/io.hpp>
#include <boost/fusion/adapted/std_pair.hpp> //without this, spirit can't accept the type with std::pair

#include <boost/spirit/include/karma.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>

namespace spiritParser
{

namespace karma = boost::spirit::karma;
namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;

struct transformTimeLabels
{
    size_t index;
    std::vector<std::vector<int>> nums;
};

}

//we need boost fusion adapt struct to expose the data structure to fusion
//else the spirit can't accept the type "transformTimeLabels
BOOST_FUSION_ADAPT_STRUCT(
        spiritParser::transformTimeLabels,
        (size_t, index)
        (std::vector<std::vector<int>>, nums)
        )

namespace spiritParser
{

    template<typename Iterator, typename T>
    bool parse_video_data(Iterator begin, Iterator end, T &data)
    {
        bool r = qi::parse(begin, end, (qi::float_ >> qi::skip["\">"] >> *~qi::char_('\n')) % *(qi::eol), data);

        if(!r || begin != end){
            return false;
        }

        return r;
    }

    template<typename OutputIterator>
    struct videoGrammar : karma::grammar<OutputIterator, transformTimeLabels()>
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

        karma::rule<OutputIterator, transformTimeLabels()> final_rule;
        karma::rule<OutputIterator, std::vector<std::vector<int>>()> second_rule;
        karma::rule<OutputIterator, std::vector<int>()> first_rule;
    };

    inline void generate_nums(std::vector<int> &nums, float data)
    {
        nums[0] = static_cast<int>(data / 3600);
        nums[1] = static_cast<int>(std::floor(data) / 60) % 60;
        nums[2] = static_cast<int>(std::floor(data)) % 60;
        nums[3] = static_cast<int>((data - std::floor(data)) * 1000.0);
    }

    /**
 *  fir = 0.5, sec = 5.19
 *  transform to : 00:00:00,500 --> 00:00:05,190
 */
    template<typename Iterator, typename Grammar>
    bool generate_times(Iterator sink, Grammar &grammar, transformTimeLabels &data, size_t index, float fir, float sec)
    {        
        data.index = index;
        generate_nums(data.nums[0], fir);
        generate_nums(data.nums[1], sec);
        bool r = karma::generate(sink,
                                 grammar,
                                 data
                                 );

        return r;
    }

}

void test_exercise_00();

#endif // EXERCISE00_HPP
