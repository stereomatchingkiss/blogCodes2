#include "moveLibs.hpp"

#define BOOST_FILESYSTEM_NO_DEPRECATED
#include <boost/filesystem.hpp>

void move_libs(std::string const &target, std::set<std::string> const &filter)
{
    namespace bf = boost::filesystem;
    using RecurSiveDirIter = bf::recursive_directory_iterator;

    boost::filesystem::create_directory(target);

    std::string const &CurrentPath = bf::current_path().generic_string();
    for(RecurSiveDirIter dir(CurrentPath), end; dir != end; ++dir){
        if(boost::filesystem::is_regular_file(*dir)){
            bf::path info(*dir);
            if(filter.find(info.extension().generic_string()) != std::end(filter)){
                auto const TargetDir = CurrentPath + "/" + target + "/";
                boost::filesystem::rename(*dir,
                                          TargetDir + info.filename().generic_string());
            }
        }
    }

    std::cout<<"process end, press any key to close"<<std::endl;
    std::cin.get();
}
