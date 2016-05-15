#include "read_usc_pedestrian.hpp"

#include <ocv_libs/file/utility.hpp>

#include <boost/algorithm/string.hpp>

#include <iostream>

std::vector<cv::Rect> read_usc_pedestrian::
parse_file(std::string const &file)
{
    using namespace pugi;

    if(!doc_.load_file(file.c_str())){
        std::string const &msg = "cannot open file : " + file;
        throw std::runtime_error(msg.c_str());
    }

    xml_node object_nodes =
            doc_.child("ObjectList");
    std::vector<cv::Rect> rects;
    for(xml_node obj_node : object_nodes.children()){
        auto node = obj_node.child("Rect");
        auto const x = node.attribute("x").as_int();
        auto const y = node.attribute("y").as_int();
        auto const width = node.attribute("width").as_int();
        auto const height = node.attribute("height").as_int();
        rects.emplace_back(cv::Rect(x, y, width, height));
    }

    return rects;
}

read_usc_pedestrian::results_type
read_usc_pedestrian::parse_folder(const std::string &folder)
{
    results_type results;

    auto const files = ocv::file::get_directory_files(folder);
    for(auto const &file : files){
        std::string img_name = file;
        boost::replace_last(img_name, ".gt.xml", ".bmp");
        results.insert({img_name,
                        parse_file(folder + "/" + file)});
    }

    return results;
}
