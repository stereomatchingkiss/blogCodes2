#ifndef BBPS_CHAR_RECOGNIZER_HPP
#define BBPS_CHAR_RECOGNIZER_HPP

#include <ocv_libs/core/block_binary_pixel_sum.hpp>

#include <opencv2/core.hpp>

#include <boost/bimap.hpp>

#include <string>

namespace cv{

namespace ml{

class StatModel;

}

}

class bbps_char_recognizer
{
public:
    /**
     * Construct object with the recognizer, the recognizer
     * expect the features extract by the class
     * block_binary_pixel_sum<float>
     * @param rec The recognizer of the character
     * @param bbps The feature extractor of the character recognizer
     */
    explicit bbps_char_recognizer(cv::Ptr<cv::ml::StatModel> rec,
                                  ocv::block_binary_pixel_sum<> const &bbps);

    /**
     * Recognize the input character
     * @param input Character before process
     * @param binary_input Character after processed,
     * should be a binary image with values, 0 or 255
     * @return The character of input should be
     */
    std::string recognize(cv::Mat const &input,
                          cv::Mat const &binary_input);

private:    
    ocv::block_binary_pixel_sum<> bbps_;
    boost::bimap<std::string, int> bimap_;
    cv::Ptr<cv::ml::StatModel> rec_;
};

#endif
