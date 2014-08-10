#include "getFilePaths.hpp"

#define BOOST_FILESYSTEM_NO_DEPRECATED
#include <boost/filesystem.hpp>

std::vector<std::string> get_file_paths_r(const std::string &path,
                                          const std::set<std::string> &select)
{
    namespace bf = boost::filesystem;
    using RecurSiveDirIter = bf::recursive_directory_iterator;

    std::vector<std::string> files;
    for(RecurSiveDirIter dir(path), end; dir != end; ++dir){
        if(boost::filesystem::is_regular_file(*dir)){
            bf::path info(*dir);
            if(!select.empty()){
                if(select.find(info.extension().generic_string()) != std::end(select)){
                    files.emplace_back(info.generic_string());
                }
            }else{
                files.emplace_back(info.generic_string());
            }
        }
    }

    return files;
}
