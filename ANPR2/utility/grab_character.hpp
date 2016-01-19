#ifndef GRAB_CHARACTER_HPP
#define GRAB_CHARACTER_HPP

#include <opencv2/core.hpp>

#include <map>
#include <string>

class grab_character
{
public:
    using contour_type = std::vector<cv::Point>;
    using contours_type = std::vector<contour_type>;

    grab_character();

    void grab_chars(cv::Mat const &plate,
                    contours_type const &contours);

    void set_chars_folder(std::string const &value);
    void set_chars_name(std::string const &value);

private:
    void save_char(cv::Mat const &plate, cv::Rect
                   const &rect, int key);

    std::map<std::string, size_t> char_count_;
    std::string chars_folder_;
    std::string chars_name_;
};

#endif
