#ifndef CROATIA_GENERAL_PLATE_RECOGNIZER
#define CROATIA_GENERAL_PLATE_RECOGNIZER

#include <opencv2/core.hpp>

#include <regex>
#include <string>

class croatia_general_plate_recognizer
{
public:
    using contour_type = std::vector<cv::Point>;
    using contours_type = std::vector<contour_type>;

    croatia_general_plate_recognizer();

    std::string fit(std::string const &pattern) const;
    void sort_contour(contours_type &contours) const;

private:
    std::string to_char(std::string const &str) const;
    std::string to_num(std::string const &str) const;

    std::regex pattern_;
};

#endif
