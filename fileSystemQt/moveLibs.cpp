#include "moveLibs.hpp"

#define BOOST_FILESYSTEM_NO_DEPRECATED
#include <boost/filesystem.hpp>

#include <string>

void move_libs()
{
    std::string const ParentPath = boost::filesystem::current_path().generic_string();

    boost::filesystem::recursive_directory_iterator iter;
}
