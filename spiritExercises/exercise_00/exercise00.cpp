#include "exercise00.hpp"

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
void test_exercise_00()
{
    std::string strs{"0.5\">If you are a premiere member of the lynda.com online training library, or if you\n\n"
                     "5.19\">are watching this tutorial on a DVD, you have access to the exercise files used throughout the title.\n\n"
                     "11.8\">are watching this tutorial on a DVD, you have access to the exercise files used throughout the title.\n\n"};

    std::vector<std::pair<float, std::string>> video;
    spiritParser::parse_video_data(std::begin(strs), std::end(strs), video);

    if(video.size() < 2) {
        std::cerr<<"not enough of data"<<std::endl;
        return;
    }

    std::string number(31, '0');
    std::string result{std::move(strs)};
    result.clear();
    std::back_insert_iterator<std::string> sink(number);
    size_t const Size = video.size() - 1;
    //use karma to generate the time label is a little bit overkill
    //sprintf or stringstream should be able to do the job(even it
    //may slower than spirit).However, for practice, I chose
    //spirit::karma to generate the time label
    spiritParser::videoGrammar<std::back_insert_iterator<std::string>> grammar;
    spiritParser::transformTimeLabels data{0, std::vector<std::vector<int>>(2, std::vector<int>(4))};
    for(size_t i = 0; i != Size; ++i){
        number.clear();
        spiritParser::generate_times(sink, grammar, data, i + 1, video[i].first, video[i + 1].first);
        result += number;
        result += video[i].second;
        result += "\n\n";
    }

    std::cout<<result<<std::endl;
}

