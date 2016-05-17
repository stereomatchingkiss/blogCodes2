#ifndef READ_STANFORD40_POSE_HPP
#define READ_STANFORD40_POSE_HPP

#include <pugixml/src/pugixml.hpp>

#include <opencv2/core.hpp>

#include <map>
#include <string>
#include <vector>

class read_stanford40_pose
{
public:
    using results_type = std::map<std::string,
    std::vector<cv::Rect>>;

    std::vector<cv::Rect> parse_file(std::string const &file);
    results_type parse_folder(std::string const &folder);

private:
    pugi::xml_document doc_;
};

#endif //READ_STANFORD40_POSE_HPP
