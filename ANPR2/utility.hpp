#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <opencv2/core.hpp>

#include <string>

void binarize_image(cv::Mat const &input,
                    cv::Mat &output);

cv::Mat binarize_image(cv::Mat const &input);

template<typename Map>
void generate_map(Map &map)
{
    int index = 0;
    for(char i = '0'; i <= '9'; ++i){
        map.insert({std::string(1,i), index++});
    }
    for(char i = 'a'; i <= 'z'; ++i){
        map.insert({std::string(1,i), index++});
    }
}

#endif
