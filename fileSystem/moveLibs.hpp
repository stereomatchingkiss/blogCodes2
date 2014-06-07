#ifndef MOVELIBS_HPP
#define MOVELIBS_HPP

#include <set>
#include <string>

void move_libs(std::string const &create_dir = "libsAndDLL",
               std::set<std::string> const &filter = {".dll", ".a", ".lib"});

#endif // MOVELIBS_HPP
