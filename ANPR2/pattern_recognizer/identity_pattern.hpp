#ifndef IDENTITY_PATTERN_HPP
#define IDENTITY_PATTERN_HPP

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

#include <vector>

class identity_pattern_recognizer
{
public:
    using contour_type = std::vector<cv::Point>;
    using contours_type = std::vector<contour_type>;

    std::string fit(std::string const &pattern) const
    {
        return pattern;
    }

    void sort_contour(contours_type &contours) const
    {
        std::sort(std::begin(contours), std::end(contours),
                  [](auto const &lhs, auto const &rhs)
        {
            return cv::boundingRect(lhs).x < cv::boundingRect(rhs).x;
        });
    }
};

#endif
