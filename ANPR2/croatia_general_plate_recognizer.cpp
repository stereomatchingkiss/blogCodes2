#include "croatia_general_plate_recognizer.hpp"

#include <opencv2/imgproc.hpp>

#include <algorithm>
#include <iostream>

croatia_general_plate_recognizer::
croatia_general_plate_recognizer()
{
    std::string fuzzy_char("[");
    for(char i = 'a'; i <= 'z'; ++i){
        fuzzy_char += i;
    }
    for(char i = 'A'; i <= 'Z'; ++i){
        fuzzy_char += i;
    }
    fuzzy_char += "08]";
    std::string const fuzzy_num("[0123456789ob]");
    //std::cout<<fuzzy_char<<std::endl;
    //std::cout<<fuzzy_num<<std::endl;

    pattern_ = std::regex(fuzzy_char + "{2}" + fuzzy_num +
                          "{3,4}" + fuzzy_char + "{1,2}");
}

std::string croatia_general_plate_recognizer::fit(const std::string &pattern) const
{
    for(auto it = std::sregex_iterator(std::begin(pattern),
                                       std::end(pattern), pattern_);
        it != std::sregex_iterator(); ++it){
        auto str = it->str();
        std::string result;
        switch(str.size()){
        case 6:
        {
            // /w{2}/d{3}/w{1}
            result += to_char(str.substr(0,2));
            result += to_num(str.substr(2,3));
            result += to_char(str.substr(5,1));
            return result;
        }
        case 7:
        {
            result += to_char(str.substr(0,2));
            result += to_num(str.substr(2,3));
            result += str.substr(5,1);
            result += to_char(str.substr(6,1));
            return result;
        }
        case 8:
        {
            result += to_char(str.substr(0,2));
            result += to_num(str.substr(2,4));
            result += to_char(str.substr(6,2));
            return result;
        }
        }
    }

    return {};
}

void croatia_general_plate_recognizer::
sort_contour(contours_type &contours) const
{
    std::sort(std::begin(contours), std::end(contours),
              [](auto const &lhs, auto const &rhs)
    {
        return cv::boundingRect(lhs).x < cv::boundingRect(rhs).x;
    });
}

std::string croatia_general_plate_recognizer::
to_char(const std::string &str) const
{
    std::string result = str;
    for(auto &ch : result){
        if(ch == '0'){
            ch = 'O';
        }else if(ch == '8'){
            ch = 'B';
        }
    }

    return result;
}

std::string croatia_general_plate_recognizer::
to_num(const std::string &str) const
{
    std::string result = str;
    for(auto &ch : result){
        if(ch == 'o' || ch == 'O'){
            ch = '0';
        }else if(ch == 'b' || ch == 'B'){
            ch = '8';
        }
    }

    return result;
}
