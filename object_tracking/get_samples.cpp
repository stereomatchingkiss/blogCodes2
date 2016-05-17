#include "get_samples.hpp"
#include "read_stanford40_pose.hpp"
#include "read_usc_pedestrian.hpp"

#include <ocv_libs/file/utility.hpp>

#include <boost/algorithm/string.hpp>

#include <opencv2/highgui.hpp>

#include <iostream>
#include <random>

namespace{

template<typename Reader>
std::vector<cv::Mat>
get_images(std::string const &folder,
           std::string const &img_sub_folder,
           size_t extract_size,
           std::function<void(cv::Mat&)> preprocess,
           unsigned int seed)
{
    Reader reader;
    std::vector<cv::Mat> results;
    std::vector<std::string> img_path;

    auto const files =
            ocv::file::get_directory_files(folder + "/" +
                                           img_sub_folder);
    for(auto const &file : files){
        img_path.emplace_back(file);
    }

    std::shuffle(std::begin(img_path), std::end(img_path),
                 std::mt19937(seed));
    img_path.resize(img_path.size() > extract_size ? extract_size :
                                                     img_path.size());
    auto const prefix = folder + "/" +
            img_sub_folder + "/";
    auto const maps = reader.parse_folder(folder);
    for(auto const &name : img_path){
        //std::cout<<name<<std::endl;
        auto const img = cv::imread(prefix + name);
        if(!img.empty()){
            auto it = maps.find(name);
            if(it != std::end(maps)){
                auto const &rects = it->second;
                for(auto const &rect : rects){
                    auto region = img(rect).clone();
                    preprocess(region);
                    results.emplace_back(region);
                }
            }
        }
    }//*/

    return results;
}

}

std::vector<cv::Mat>
get_stanford40_pose(std::string const &folder,
                    size_t extract_size,
                    std::function<void(cv::Mat&)> preprocess,
                    unsigned int seed)
{
    return get_images<read_stanford40_pose>(folder,
                                            "JPEGImages",
                                            extract_size,
                                            preprocess,
                                            seed);
}

std::vector<cv::Mat>
get_usc_pedestrian(std::string const &folder,
                   size_t extract_size,
                   std::function<void(cv::Mat&)> preprocess,
                   unsigned int seed)
{
    return get_images<read_usc_pedestrian>(folder,
                                           "img",
                                           extract_size,
                                           preprocess,
                                           seed);
}
