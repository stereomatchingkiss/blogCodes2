#include "get_samples.hpp"
#include "read_usc_pedestrian.hpp"

#include <ocv_libs/file/utility.hpp>

#include <boost/algorithm/string.hpp>

#include <opencv2/highgui.hpp>

#include <iostream>

std::vector<cv::Mat>
get_usc_pedestrian(std::vector<std::string> const &folders)
{
    read_usc_pedestrian rup;    
    std::vector<cv::Mat> results;

    for(auto &folder : folders){
        auto maps = rup.parse_folder(folder);
        auto const files =
                ocv::file::get_directory_files(folder + "/img");
        for(auto const &file : files){            
            //std::cout<<folder + "/img/" + file<<std::endl;
            auto const img = cv::imread(folder + "/img/" + file,
                                        cv::IMREAD_GRAYSCALE);
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
