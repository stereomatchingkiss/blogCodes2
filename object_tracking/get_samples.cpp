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

std::vector<cv::Mat>
get_images(std::string const &folder,
           size_t extract_size,
           std::function<void(cv::Mat&)> preprocess,
           unsigned int seed)
{
    std::vector<std::string> files;
    for(auto const &fd : ocv::file::get_directory_folders(folder)){
        //std::cout<<"folder : "<<fd<<std::endl;
        auto names = ocv::file::get_directory_files(folder + "/" + fd);
        for(auto const &name : names){
            files.emplace_back(fd + "/" + name);
        }
    }
    for(auto &&file : ocv::file::get_directory_files(folder)){
        files.emplace_back(file);
    }

    //std::cout<<"files size : "<<files.size()<<std::endl;
    std::shuffle(std::begin(files), std::end(files),
                 std::mt19937(seed));
    files.resize(files.size() > extract_size ? extract_size :
                                               files.size());
    std::vector<cv::Mat> imgs;
    for(auto const &file : files){
        auto img = cv::imread(folder + "/" + file);
        if(!img.empty()){
            preprocess(img);
            imgs.emplace_back(img);
        }else{
            std::cout<<"cannot open : "<<file<<std::endl;
        }
    }

    return imgs;
}

} //namespace

std::vector<cv::Mat> get_caltech_bg(const std::string &folder,
                                    size_t extract_size,
                                    std::function<void (cv::Mat &)> preprocess,
                                    unsigned int seed)
{
    return get_images(folder, extract_size, preprocess, seed);
}

std::vector<cv::Mat>
get_indoor_scene_cvpr2009(const std::string &folder,
                          size_t extract_size,
                          std::function<void (cv::Mat &)> preprocess,
                          unsigned int seed)
{
    return get_images(folder, extract_size, preprocess, seed);
}

void get_indoor_scene_cvpr2009(std::vector<cv::Mat> &inout,
                               const std::string &folder,
                               size_t extract_size,
                               std::function<void (cv::Mat &)> preprocess,
                               unsigned int seed)
{
    auto const imgs = get_indoor_scene_cvpr2009(folder, extract_size,
                                                preprocess, seed);
    std::copy(std::begin(imgs), std::end(imgs), std::back_inserter(inout));
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

void
get_stanford40_pose(std::vector<cv::Mat> &inout,
                    const std::string &folder,
                    size_t extract_size,
                    std::function<void (cv::Mat &)> preprocess,
                    unsigned int seed)
{
    auto const img =
            get_stanford40_pose(folder, extract_size,
                                preprocess, seed);
    std::copy(std::begin(img), std::end(img), std::back_inserter(inout));
}

void get_usc_pedestrian(std::vector<cv::Mat> &inout,
                        const std::string &folder,
                        size_t extract_size,
                        std::function<void (cv::Mat &)> preprocess,
                        unsigned int seed)
{
    auto const img =
            get_usc_pedestrian(folder, extract_size,
                               preprocess, seed);
    std::copy(std::begin(img), std::end(img), std::back_inserter(inout));
}

void get_caltech_bg(std::vector<cv::Mat> &inout,
                    const std::string &folder,
                    size_t extract_size,
                    std::function<void (cv::Mat &)> preprocess,
                    unsigned int seed)
{
    auto const img =
            get_caltech_bg(folder, extract_size,
                           preprocess, seed);
    std::copy(std::begin(img), std::end(img), std::back_inserter(inout));
}
