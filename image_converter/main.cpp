#include <ocv_libs/core/resize.hpp>
#include <ocv_libs/file/utility.hpp>

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <iostream>

using image_pair = std::vector<std::pair<cv::Mat, std::string>>;

image_pair convert_color(std::vector<std::string> const &image_names,
                         std::string const &input_folder,
                         std::string const &from_color_space,
                         std::string const &to_color_space);

std::vector<std::string> get_image_names(std::string const &folder);

boost::program_options::variables_map parse_command_line(int argc, char **argv);

void rgb_to_gray(std::vector<std::string> const &image_names,
                 std::string const &input_folder,
                 int cvt_color_space,
                 image_pair &outputs);

void rgb_to_intensity(std::vector<std::string> const &image_names,
                      std::string const &input_folder,
                      image_pair &outputs);

int main(int argc, char **argv)
{
    try{
        auto const map = parse_command_line(argc, argv);
        if(!map.count("help")){
            auto const input_folder = map["input_folder"].as<std::string>();
            auto const image_names =
                    get_image_names(input_folder);

            auto const from_color_space = map["from_color_space"].as<std::string>();
            auto const to_color_space = map["to_color_space"].as<std::string>();
            auto const output_images =
                    convert_color(image_names, input_folder,
                                  from_color_space, to_color_space);

            auto const output_folder = map["output_folder"].as<std::string>() + "/";
            cv::Size const size((int)map["output_width"].as<size_t>(),
                    (int)map["output_height"].as<size_t>());
            cv::Mat resize_img;
            for(auto const &pair : output_images){
                std::cout<<"save image : "<<(output_folder + pair.second)<<std::endl;
                ocv::resize_aspect_ratio(pair.first, resize_img, size);
                cv::imwrite(output_folder + pair.second, resize_img);
            }
        }
    }catch(std::exception const &ex){
        std::cerr<<ex.what()<<std::endl;
    }
}

image_pair convert_color(std::vector<std::string> const &image_names,
                         std::string const &input_folder,
                         std::string const &from_color_space,
                         std::string const &to_color_space)
{
    enum class transform_method
    {
        rgb2gray,
        rgb2int
    };

    std::map<std::pair<std::string, std::string>, transform_method> const color_space_map
    {
        {{"rgb", "gray"}, transform_method::rgb2gray},
        {{"rgb", "intensity"}, transform_method::rgb2int},
    };

    image_pair outputs;
    switch(color_space_map.at({from_color_space, to_color_space})){
    case transform_method::rgb2gray:{
        std::cout<<"rgb to gray"<<std::endl;
        rgb_to_gray(image_names, input_folder, CV_RGB2GRAY, outputs);
        break;
    }
    case transform_method::rgb2int:{
        std::cout<<"rgb to intensity"<<std::endl;
        rgb_to_intensity(image_names, input_folder, outputs);
        break;
    }
    default:{
        std::cerr<<"("<<from_color_space<<", "<<
                   to_color_space<<
                   ") do not match any conversion rule\n";
        break;
    }
    }

    return outputs;
}

std::vector<std::string> get_image_names(std::string const &folder)
{
    auto images_name = ocv::file::get_directory_files(folder);
    auto is_non_valid_extension = [](std::string const &name)
    {
        boost::filesystem::path path(name);
        std::string const extension(path.extension().generic_string());
        if(boost::iequals(extension, ".bmp") ||
                boost::iequals(extension, ".jpg") ||
                boost::iequals(extension, ".png")){
            return false;
        }

        return true;
    };
    auto it = std::remove_if(std::begin(images_name), std::end(images_name),
                             is_non_valid_extension);
    images_name.erase(it, std::end(images_name));

    return images_name;
}

boost::program_options::variables_map parse_command_line(int argc, char **argv)
{
    using namespace boost::program_options;

    options_description desc{"options"};

    std::string const size_comments =
            "If you only specify width or height, "
            "the app will keep the aspect ratio; If you specify "
            "both of them, it would ignore aspect ratio;If you "
            "ignore both of them, it would keep the original size";

    desc.add_options()
            ("help,h", "Help menu")
            ("input_folder,i", value<std::string>()->required(),
             "The folder of images, support images with extensions [jpg, bmp, png]")
            ("output_folder,o", value<std::string>()->required(),
             "Output folder of images")
            ("from_color_space,f", value<std::string>()->default_value("rgb"),
             "The color space of input images, support [rgb]")
            ("to_color_space,t",value<std::string>()->default_value("gray"),
             "The color space of output images, support [gray, intensity]."
             "[intensity] is the v channel of hsv")
            ("output_width,W", value<size_t>()->default_value(0),
             ("Width of the output images;" + size_comments).c_str())
            ("output_height,H", value<size_t>()->default_value(0),
             ("Height of the output images;" + size_comments).c_str());

    variables_map vm;
    store(parse_command_line(argc, argv, desc), vm);

    if(vm.count("help")){
        std::cout<<desc<<std::endl;
        return {};
    }

    if(!boost::filesystem::exists(vm["input_folder"].as<std::string>())){
        std::cerr<<"cannot find the input folder\n";
    }

    if(!boost::filesystem::exists(vm["output_folder"].as<std::string>())){
        throw std::runtime_error("cannot find the output folder");
    }

    //the magic to make the other required arguments
    //become optional if the users input "help"
    notify(vm);

    return vm;
}

void rgb_to_gray(std::vector<std::string> const &image_names,
                 std::string const &input_folder,
                 int cvt_color_space,
                 image_pair &outputs)
{
    auto const folder = input_folder + "/";
    for(size_t i = 0; i != image_names.size(); ++i){
        cv::Mat img = cv::imread(folder + image_names[i]);
        if(!img.empty()){
            outputs.emplace_back(cv::Mat(), image_names[i]);
            cv::cvtColor(img, outputs.back().first, cvt_color_space);
        }else{
            std::cerr<<(folder + image_names[i])<<" cannot open"<<std::endl;
        }
    }
}

void rgb_to_intensity(std::vector<std::string> const &image_names,
                      std::string const &input_folder,
                      image_pair &outputs)
{
    std::vector<cv::Mat> splitted;
    cv::Mat hsv;
    auto const folder = input_folder + "/";
    for(size_t i = 0; i != image_names.size(); ++i){
        cv::Mat img = cv::imread(folder + image_names[i]);
        if(!img.empty()){
            cv::cvtColor(img, hsv, CV_BGR2HSV);
            cv::split(hsv, splitted);
            outputs.emplace_back(splitted[2].clone(), image_names[i]);
        }else{
            std::cerr<<(folder + image_names[i])<<" cannot be load "<<std::endl;
        }
    }
}
