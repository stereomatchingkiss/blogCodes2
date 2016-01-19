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

/**
 * Recognize plate with 6 characters, the pattern is
 * w{2}d{3}w{1}
 */
class bbps_char_recognizer
{
public:    
    using regions_type = std::vector<cv::Rect>;

    /**
     * Construct object with the recognizer, the recognizer
     * expect the features extract by the class
     * block_binary_pixel_sum<float>
     * @param alpha_rec The recognizer of alphabet
     * @param num_rec The recognizer of number
     * @param bbps The feature extractor of the character recognizer
     */
    explicit bbps_char_recognizer(cv::Ptr<cv::ml::StatModel> alpha_rec,
                                  cv::Ptr<cv::ml::StatModel> num_rec,
                                  ocv::block_binary_pixel_sum<> const &bbps = ocv::block_binary_pixel_sum<>());    

    /**
     * Recognize the input character
     * @param input Character before process
     * @param binary_input Character after processed,
     * should be a binary image with values, 0 or 255
     * @param regions regions of characters
     * @return The character of input should be
     */
    std::string recognize(cv::Mat const &input,
                          cv::Mat const &binary_input,
                          regions_type const &regions);

private:    
    std::string recognize(cv::Mat const &character,
                          cv::Ptr<cv::ml::StatModel> const &rec);

    cv::Ptr<cv::ml::StatModel> alpha_rec_;
    ocv::block_binary_pixel_sum<> bbps_;
    boost::bimap<std::string, int> bimap_;
    cv::Ptr<cv::ml::StatModel> num_rec_;
};

#endif
