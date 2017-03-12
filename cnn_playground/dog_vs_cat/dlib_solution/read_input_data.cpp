#include "read_input_data.hpp"

#include "global_var.hpp"

using namespace dlib;
using namespace std;

std::vector<image_info> read_img_info(std::string const &folder)
{
    auto files = directory(folder).get_files();
    std::sort(std::begin(files), std::end(files), [](auto const &lhs, auto const &rhs)
    {
        return lhs.name() < rhs.name();
    });
    std::vector<image_info> img_info;
    for(auto const &file : files){
        auto const tag = file.name().find("cat.") != std::string::npos ?
                    global::cat_label : global::dog_label;
        img_info.emplace_back(file.full_name(), tag);
    }

    return img_info;
}

image_info::image_info(string filename, int numeric_label) :
    filename_(std::move(filename)),
    numeric_label_(numeric_label)
{}

void read_image_info_from_file(const string &file, std::vector<image_info> &output)
{
    std::ifstream in(file);
    if(!in.is_open()){
        throw std::runtime_error("read_image_info_from_file cannot open file");
    }

    std::string line;
    while(std::getline(in, line)){
        std::istringstream stream(line);
        std::string filename;
        int numeric_label;
        stream>>numeric_label>>filename;
        output.emplace_back(std::move(filename), numeric_label);
    }
}

std::vector<image_info> read_image_info_from_file(const string &file)
{
    std::vector<image_info> output;
    read_image_info_from_file(file, output);

    return output;
}
