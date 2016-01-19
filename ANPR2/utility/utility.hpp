#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <opencv2/core.hpp>

#include <string>

enum class map_type
{
    alpahbet,
    number,
    alpha_num,
};

void binarize_image(cv::Mat const &input,
                    cv::Mat &output);

cv::Mat binarize_image(cv::Mat const &input);

template<typename Map>
int generate_number_map(Map &map)
{
    int index = 0;
    for(char i = '0'; i <= '9'; ++i){
        map.insert({std::string(1,i), index++});
    }

    return index;
}

template<typename Map>
void generate_char_map(Map &map, int index)
{
    for(char i = 'a'; i < 'q'; ++i){
        map.insert({std::string(1,i), index++});
    }
    for(char i = 'r'; i < 'w'; ++i){
        map.insert({std::string(1,i), index++});
    }
    map.insert({std::string(1,'z'), index++});
}

template<typename Map>
void generate_map(Map &map, map_type mtype = map_type::alpha_num)
{
    if(mtype == map_type::number){
        generate_number_map(map);
    }else if(mtype == map_type::alpahbet){
        generate_char_map(map, 0);
    }else{
        int const index = generate_number_map(map);
        generate_char_map(map, index);
    }
}

#endif
