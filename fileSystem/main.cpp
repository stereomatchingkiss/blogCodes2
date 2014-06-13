#include "moveLibs.hpp"

#define BOOST_FILESYSTEM_NO_DEPRECATED
#include <boost/filesystem.hpp>

#include <iostream>
#include <set>
#include <string>

int main(int argc, char *argv[])
{    
    if(argc == 1){
        move_libs();
    }else{        
        std::set<std::string> filter;
        if(argc > 2){
            for(int i = 2; i != argc; ++i){
                filter.emplace(argv[i]);
            }
        }
        move_libs(argv[1], filter);
    }

    return 0;
}

