#ifndef DETECTCHARACTER_HPP
#define DETECTCHARACTER_HPP

#include <vector>

#include <opencv2/core/core.hpp>

/**
 * @brief detect the digits from image.
 */

class detectCharacter
{
public:
    detectCharacter();
    detectCharacter(detectCharacter const&) = delete;
    detectCharacter& operator=(detectCharacter const&) = delete;
    detectCharacter(detectCharacter &&data) noexcept;
    detectCharacter& operator=(detectCharacter &&data) noexcept;

    std::vector<cv::Mat> segment(cv::Mat const &input, cv::Size const &crop_size);

private:
    void clear();

    cv::Mat crop_digit(cv::Mat const &input, cv::Rect const &rect, std::vector<cv::Point> const &contour, cv::Size const &size);

private:
    std::vector<std::vector<cv::Point>> contours;
    cv::Mat gray;
    std::vector<cv::Rect> min_rects;
};

#endif // DETECTCHARACTER_HPP
