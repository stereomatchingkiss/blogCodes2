#include "get_samples.hpp"
#include "read_stanford40_pose.hpp"
#include "read_usc_pedestrian.hpp"

#include <ocv_libs/file/utility.hpp>

#include <boost/algorithm/string.hpp>

#include <opencv2/highgui.hpp>

#include <iostream>

namespace{

template<typename Reader>
std::vector<cv::Mat>
get_images(std::vector<std::string> const &folders,
           std::string const &img_sub_folder)
{
    Reader reader;
    std::vector<cv::Mat> results;

    for(auto &folder : folders){
        auto maps = reader.parse_folder(folder);
        auto const files =
                ocv::file::get_directory_files(folder + "/" +
                                               img_sub_folder);
        for(auto const &file : files){
            auto const img = cv::imread(folder + "/" +
                                        img_sub_folder +
                                        "/" + file);
            if(!img.empty()){
                auto it = maps.find(file);
                if(it != std::end(maps)){
                    auto const &rects = it->second;
                    for(auto const &rect : rects){
                        results.emplace_back(img(rect).clone());
                    }
                }
            }
        }
    }

    return results;
}

}

std::vector<cv::Mat>
get_stanford40_pose(const std::vector<std::string> &folders)
{
    return get_images<read_stanford40_pose>(folders,
                                            "JPEGImages");
}

std::vector<cv::Mat>
get_usc_pedestrian(std::vector<std::string> const &folders)
{
    return get_images<read_usc_pedestrian>(folders, "img");
}
