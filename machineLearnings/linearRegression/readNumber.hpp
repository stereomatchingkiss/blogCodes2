#ifndef READNUMBER_HPP
#define READNUMBER_HPP

#include <iostream>
#include <fstream>

#include <opencv2/core/core.hpp>

/**
 * @brief read number from the file
 * @param name name of the file
 * @return the numbers
 */
template<typename T = float>
cv::Mat_<T> read_number(std::string const &name)
{
    std::ifstream in(name);

    if(!in.is_open()){
        std::cerr<<"can't open file : "<<name<<std::endl;
        return cv::Mat_<T>();
    }

    cv::Mat_<T> features;
    T data;
    while(in >> data){
        features.push_back(data);
    }

    return features;
}

#endif // READNUMBER_HPP
