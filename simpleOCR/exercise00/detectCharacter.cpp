#include <iostream>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "detectCharacter.hpp"

detectCharacter::detectCharacter()
{
}

detectCharacter::detectCharacter(detectCharacter &&data) noexcept
{
    *this = std::move(data);
}

detectCharacter& detectCharacter::operator=(detectCharacter &&data) noexcept
{
    contours = std::move(data.contours);
    gray = data.gray;

    return *this;
}

/**
 * @brief cut out the charecters in the input
 * @param input : input image
 * @param crop_size : the character size after cropping
 * @return the characters after segmentation
 */
std::vector<cv::Mat> detectCharacter::segment(cv::Mat const &input, cv::Size const &crop_size)
{
    CV_Assert(input.type() == CV_8UC3);

    clear();

    cv::cvtColor(input, gray, CV_BGR2GRAY);
    cv::GaussianBlur(gray, gray, cv::Size(5, 5), 0);

    cv::Mat binary = gray;
    cv::threshold(gray, binary, 0, 255, cv::THRESH_BINARY_INV + cv::THRESH_OTSU);

    //cv::imshow("binary", binary);
    //cv::waitKey();

    cv::findContours(binary.clone(), contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
    //std::cout<<"contours size = "<<contours.size()<<std::endl;

    for(auto &data : contours){
        min_rects.emplace_back(cv::boundingRect(data));
    }

    std::vector<cv::Mat> digits;
    for(size_t i = 0, size = contours.size(); i != size; ++i){
        cv::Mat const digit = crop_digit(binary, min_rects[i], contours[i], crop_size);
        digits.emplace_back(digit);
    }

    return digits;
}

/**********************************************************
 ****************** implementation ************************
 **********************************************************/

void detectCharacter::clear()
{
    contours.clear();
    min_rects.clear();
}

/**
 * @brief crop a character in the input
 * @param input : input image
 * @param rect  : location of the character
 * @param points : contours
 * @param size : size of the result
 * @return character after crop
 */
cv::Mat detectCharacter::crop_digit(cv::Mat const &input, cv::Rect const &rect, std::vector<cv::Point> const &contour, cv::Size const &size)
{
    cv::Mat mask = cv::Mat(input, rect);
    cv::Mat digit(mask.size(), mask.type());
    digit.setTo(0);

    auto digit_ptr = digit.ptr<uchar>(0);
    for(int row = 0; row != mask.rows; ++row){
        auto mask_ptr = mask.ptr<uchar>(row);
        for(int col = 0; col != mask.cols; ++col){
            if(*mask_ptr == 255 && cv::pointPolygonTest(contour, {col + rect.x, row + rect.y}, false) >= 0){
                *digit_ptr = 255;
            }
            ++mask_ptr; ++digit_ptr;
        }
    }

    cv::resize(digit, digit, size);
    //cv::equalizeHist(digit, digit);

    return digit;
}
