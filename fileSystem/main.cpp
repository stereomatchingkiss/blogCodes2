#include "getFilePaths.hpp"
#include "moveLibs.hpp"

#include <boost/algorithm/string/find.hpp>

#define BOOST_FILESYSTEM_NO_DEPRECATED
#include <boost/filesystem.hpp>

#include <fstream>
#include <iostream>
#include <set>
#include <string>

void generate_files_of_wlg_project()
{
    auto const CurrentPath = boost::filesystem::current_path().generic_string();
    auto const Result = get_file_paths_r(CurrentPath, {".c", ".cc", ".cpp", ".h", ".hpp"});

    std::ofstream header("header.txt");
    std::ofstream impl("impl.txt");
    for(auto const &Str : Result){
        boost::filesystem::path path(Str);

        if(!boost::algorithm::find_last(Str, "T-i386-bsd") &&
                !boost::algorithm::find_last(Str, "T-i386-ntvc") &&
                !boost::algorithm::find_last(Str, "T-i386-sol") &&
                !boost::algorithm::find_last(Str, "T-mips-bsd") &&
                !boost::algorithm::find_last(Str, "T-mips-bsd32") &&
                !boost::algorithm::find_last(Str, "T-mips-linux") &&
                !boost::algorithm::find_last(Str, "T-sparc-sol")){

            auto const &Extension = path.extension().generic_string();
            if(Extension == ".c" || Extension == ".cc" || Extension == ".cpp"){
                impl<<path.generic_string()<<std::endl;
            }else{
                header<<path.generic_string()<<std::endl;
            }
        }
    }
}

int main(int argc, char *argv[])
{    
    generate_files_of_wlg_project();

    /*if(argc == 1){
        //move_libs();
        auto const Result = get_file_paths_r
                (boost::filesystem::current_path().generic_string(),
        {".cpp", ".h", ".hpp"});
        for(auto const &Str : Result){
            std::cout<<Str<<std::endl;
        }
        std::cin.get();
    }else{
        std::set<std::string> filter;
        if(argc > 2){
            for(int i = 2; i != argc; ++i){
                filter.emplace(argv[i]);
            }
        }
        move_libs(argv[1], filter);
    }*/

    return 0;
}

