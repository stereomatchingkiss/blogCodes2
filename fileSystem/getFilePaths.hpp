#ifndef GETFILEPATHS_HPP
#define GETFILEPATHS_HPP

#include <set>
#include <string>
#include <vector>

std::vector<std::string> get_file_paths_r(std::string const &path,
                                          std::set<std::string> const &select = {});

#endif // GETFILEPATHS_HPP
