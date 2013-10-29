#ifndef BASICIMAGEALGO_HPP
#define BASICIMAGEALGO_HPP

#include <stdexcept>
#include <string>

#include <opencv2/core/core.hpp>

#include "bugLocation.hpp"

namespace OCV
{

template<typename T, typename UnaryFunc, typename Mat>
UnaryFunc for_each_continuous_channel(Mat &&input, int channel, UnaryFunc func);

template<typename T, typename UnaryFunc, typename Mat>
UnaryFunc for_each_continuous_channels(Mat &&input, UnaryFunc func);

/**
 *@brief implement details of for_each_channel, user should not use this function
 */
template<typename T, typename UnaryFunc, typename Mat>
UnaryFunc for_each_channel_impl(Mat &&input, int channel, UnaryFunc func)
{
    int const rows = input.rows;
    int const cols = input.cols;
    int const channels = input.channels();
    for(int row = 0; row != rows; ++row){
        auto input_ptr = input.template ptr<T>(row) + channel;
        for(int col = 0; col != cols; ++col){
            func(*input_ptr);
            input_ptr += channels;
        }
    }

    return func;
}

/**
 *@brief apply stl like for_each algorithm on a channel
 *
 * @param T : the type of the channel(ex, uchar, float, double and so on)
 * @param channel : the channel need to apply for_each algorithm
 * @param func : Unary function that accepts an element in the range as argument
 *
 *@return :
 *  return func
 */
template<typename T, typename UnaryFunc, typename Mat>
inline UnaryFunc for_each_channel(Mat &&input, int channel, UnaryFunc func)
{   
    if(input.isContinuous()){
        if(input.channels() == 1){
            return for_each_continuous_channels<T>(std::forward<Mat>(input), func);
        }else{
            return for_each_continuous_channel<T>(std::forward<Mat>(input), channel, func);
        }
    }else{
        return for_each_channel_impl<T>(std::forward<Mat>(input), channel, func);
    }
}

/**
 *@brief apply stl like for_each algorithm on a channel
 *
 * @param T : the type of the channel(ex, uchar, float, double and so on)
 * @param func : Unary function that accepts an element in the range as argument
 *
 *@return :
 *  return func
 */
template<typename T, typename UnaryFunc, typename Mat>
UnaryFunc for_each_channels(Mat &&input, UnaryFunc func)
{   
    int rows = input.rows;
    int cols = input.cols;

    if(input.isContinuous()){
        cols = input.total() * input.channels();
        rows = 1;
    }

    for(int row = 0; row != rows; ++row){
        auto input_ptr = input.template ptr<T>(row);
        for(int col = 0; col != cols; ++col){
            func(input_ptr[col]);
        }
    }

    return func;
}

template<typename T, typename BinaryFunc, typename Mat>
BinaryFunc for_each_channels(Mat &&input_1, Mat &&input_2, BinaryFunc func)
{
    if(input_1.rows != input_2.rows || input_1.cols != input_2.cols || input_1.type() != input_2.type()){
        throw std::runtime_error(COMMON_DEBUG_MESSAGE + "input_1.rows != input_2.rows || input_1.cols != input_2.cols || input_1.type() != input_2.type()");
    }

    int rows = input_1.rows;
    int cols = input_1.cols;

    if(input_1.isContinuous() && input_2.isContinuous()){
        rows = 1;
        cols = input_1.total() * input_1.channels();
    }

    for(int row = 0; row != rows; ++row){
        auto input_1_ptr = input_1.template ptr<T>(row);
        auto input_2_ptr = input_2.template ptr<T>(row);
        for(int col = 0; col != cols; ++col){
            func(input_1_ptr[col], input_2_ptr[col]);
            //++input_1_ptr; ++input_2_ptr;
        }
    }

    return func;
}

/**
 * @brief apply stl like for_each algorithm on a channel
 *
 * @param T : the type of the channel(ex, uchar, float, double and so on)
 * @param func : Unary function that accepts an element in the range as argument
 *
 * @return : return func
 */
template<typename T, typename UnaryFunc, typename Mat>
UnaryFunc for_each_continuous_channel(Mat &&input, int channel, UnaryFunc func)
{
    int const size = input.total();
    int const channels = input.channels();
    auto input_ptr = input.template ptr<T>(0) + channel;
    for(int i = 0; i != size; ++i){
        func(*input_ptr);
        input_ptr += channels;
    }

    return func;
}

/**
 * @brief apply stl like for_each algorithm on all channels
 *
 * @param T : the type of the channel(ex, uchar, float, double and so on)
 * @param func : Unary function that accepts an element in the range as argument
 *
 * @return : return func
 */
template<typename T, typename UnaryFunc, typename Mat>
UnaryFunc for_each_continuous_channels(Mat &&input, UnaryFunc func)
{
    int const total = input.total() * input.channels();
    auto input_ptr = input.template ptr<T>(0);
    for(int i = 0; i != total; ++i){
        func(input_ptr[i]);
        //++input_ptr;
    }

    return func;
}

}

#endif // BASIC_HPP
