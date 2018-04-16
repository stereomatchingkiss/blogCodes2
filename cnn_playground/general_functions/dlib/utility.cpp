#include "utility.hpp"

void get_dirs_recursive(dlib::directory const &dir, std::vector<dlib::directory> &output)
{
    auto dirs = dir.get_dirs();
    if(dirs.empty()){
        return;
    }else{
        output.insert(std::end(output), std::begin(dirs), std::end(dirs));
        for(auto const &d : dirs){            
            get_dirs_recursive(d, output);
        }
    }
}

std::vector<std::string> get_images_location(const std::vector<dlib::directory> &input)
{
    std::vector<std::string> results;
    for(auto const &dir : input){
        auto const files = dir.get_files();
        for(auto const &file : files){
            results.emplace_back(file.full_name());
        }
    }

    return results;
}
