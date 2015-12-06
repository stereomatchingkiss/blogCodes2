#include "fhog_trainer.hpp"
#include "morphology_localizer.hpp"

#include <ocv_libs/cmd/command_prompt_utility.hpp>
#include <ocv_libs/file/utility.hpp>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

#include <iostream>

template<typename Localizer = morphology_localizer>
void test_number_plate_localizer(int argc, char **argv);

int main(int argc, char **argv)
{               
    //fhog_number_plate_trainer fhog_trainer(argc, argv);
    test_number_plate_localizer<>(argc, argv);

}

template<typename Localizer>
void test_number_plate_localizer(int argc, char **argv)
{
    auto const map =
            ocv::cmd::default_command_line_parser(argc, argv).first;
    if(map.count("image_folder") || map.count("image")){
        bool const image_folder_exist =
                static_cast<bool>(map.count("image_folder"));
        auto const target = image_folder_exist ?
                    map["image_folder"].as<std::string>() :
                map["image"].as<std::string>();
        auto images = image_folder_exist ?
                    ocv::file::get_directory_files(target) :
                    std::vector<std::string>();
        Localizer lpl;
        lpl.set_show_debug_message(true);
        if(!images.empty()){
            for(auto const &im_name : images){
                std::cout<<(target + "/" + im_name)<<std::endl;
                cv::Mat input = cv::imread(target + "/" + im_name);
                if(!input.empty()){
                    std::vector<cv::Rect> plate_regions;
                    lpl.localize_plate(input, plate_regions);
                }
            }
        }else{
            std::cout<<target<<std::endl;
            cv::Mat input = cv::imread(target);
            if(!input.empty()){
                std::vector<cv::Rect> plate_regions;
                lpl.localize_plate(input, plate_regions);
            }
        }
    }else{
        std::cerr<<"must input folder path or image name"<<std::endl;
    }
}
