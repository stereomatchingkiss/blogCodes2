#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <dlib/dir_nav.h>

#include <vector>

void get_dirs_recursive(dlib::directory const &dir, std::vector<dlib::directory> &output);
std::vector<std::string> get_images_location(std::vector<dlib::directory> const &input);

#endif
