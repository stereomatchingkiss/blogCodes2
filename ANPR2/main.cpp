#include "fhog_trainer.hpp"
#include "morphology_localizer.hpp"
#include "segment_character.hpp"

#include <ocv_libs/cmd/command_prompt_utility.hpp>
#include <ocv_libs/file/utility.hpp>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <array>
#include <iostream>

using vmap = boost::program_options::variables_map;

template<typename UnaryFunctor>
void test_algo(vmap const &map, UnaryFunctor functor);

void test_number_plate_localizer(int argc, char **argv);
void test_segment_character(int argc, char **argv);

int main(int argc, char **argv)
{               
    //fhog_number_plate_trainer fhog_trainer(argc, argv);
    test_number_plate_localizer(argc, argv);
}

template<typename BinaryFunctor>
void test_algo(vmap const &map, BinaryFunctor functor)
{
    if(map.count("image_folder")){
        auto const target = map["image_folder"].as<std::string>();
        auto images = ocv::file::get_directory_files(target);
        for(auto const &im_name : images){
            std::cout<<(target + "/" + im_name)<<std::endl;
            cv::Mat input = cv::imread(target + "/" + im_name);
            if(!input.empty()){
                functor(input);
            }
        }
    }else if(map.count("image")){
        auto const target = map["image"].as<std::string>();
        std::cout<<target<<std::endl;
        cv::Mat input = cv::imread(target);
        if(!input.empty()){
            functor(input);
        }
    }else{
        std::cerr<<"must specify image_folder or image"<<std::endl;
    }
}

void test_number_plate_localizer(int argc, char **argv)
{    
    auto const map =
            ocv::cmd::default_command_line_parser(argc, argv).first;
    morphology_localizer lpl;
    lpl.set_show_debug_message(true);
    test_algo(map, [&](cv::Mat const &input)
    {
        lpl.localize_plate(input);
    });
}

void test_segment_character(int argc, char **argv)
{
    auto const map =
            ocv::cmd::default_command_line_parser(argc, argv).first;
    morphology_localizer lpl;
    lpl.set_show_debug_message(true);
    segment_character sc;
    test_algo(map, [&](cv::Mat const &input)
    {
        lpl.localize_plate(input);
        sc.detect_characters(input, lpl.get_contours());
    });
}
