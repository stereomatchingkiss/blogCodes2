#include "read_stanford40_pose.hpp"

#include <ocv_libs/file/utility.hpp>

#include <boost/algorithm/string.hpp>

#include <iostream>

std::vector<cv::Rect> read_stanford40_pose::
parse_file(const std::string &file)
{
    using namespace pugi;

    if(!doc_.load_file(file.c_str())){
        std::string const &msg = "cannot open file : " + file;
        throw std::runtime_error(msg.c_str());
    }

    xml_node object_nodes =
            doc_.child("annotation").child("object");
    xml_node obj_node = object_nodes.child("bndbox");

    auto const x = std::stoi(obj_node.child_value("xmin"));
    auto const y = std::stoi(obj_node.child_value("ymin"));
    auto const width = std::stoi(obj_node.child_value("xmax")) - x;
    auto const height = std::stoi(obj_node.child_value("ymax")) - y;

    std::vector<cv::Rect> rects;
    rects.emplace_back(cv::Rect(x, y, width, height));

    return rects;
}

read_stanford40_pose::results_type
read_stanford40_pose::parse_folder(const std::string &folder)
{
    using namespace ocv::file;

    results_type results;
    auto const imgs = get_directory_files(folder + "/JPEGImages");
    auto const xml = get_directory_files(folder + "/XMLAnnotations");
    if(xml.size() != imgs.size()){
        throw std::runtime_error("xml.size() != imgs.size()");
    }

    for(size_t i = 0; i != imgs.size(); ++i){
        results.insert({imgs[i],
                       parse_file(folder + "/XMLAnnotations/" + xml[i])});
    }

    return results;
}
