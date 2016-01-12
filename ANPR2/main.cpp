#include "croatia_general_plate_recognizer.hpp"
#include "fhog_trainer.hpp"
#include "grab_character.hpp"
#include "morphology_localizer.hpp"
#include "prune_illegal_chars.hpp"
#include "segment_character.hpp"
#include "train_chars.hpp"

#include <ocv_libs/cmd/command_prompt_utility.hpp>
#include <ocv_libs/core/perspective_transform.hpp>
#include <ocv_libs/file/utility.hpp>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <boost/filesystem.hpp>

#include <array>
#include <iostream>

using vmap = boost::program_options::variables_map;

template<typename UnaryFunctor>
void test_algo(vmap const &map, UnaryFunctor functor);

void test_croatia_general_recognizer();
void test_grab_char(int argc, char **argv);
void test_number_plate_localizer(int argc, char **argv);
void test_prune_illegal_chars(int argc, char **argv);
void test_segment_character(int argc, char **argv);
void test_train_chars(int argc, char **argv);

int main(int argc, char **argv)
{                   
    //fhog_number_plate_trainer fhog_trainer(argc, argv);

    test_croatia_general_recognizer();
    //test_grab_char(argc, argv);
    //test_number_plate_localizer(argc, argv);
    //test_prune_illegal_chars(argc, argv);
    //test_segment_character(argc, argv);
    //test_train_chars(argc, argv);
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
                boost::filesystem::path path(im_name);
                functor(input, path.stem().string());
            }
        }
    }else if(map.count("image")){
        auto const target = map["image"].as<std::string>();
        std::cout<<target<<std::endl;
        cv::Mat input = cv::imread(target);
        if(!input.empty()){
            boost::filesystem::path path(target);
            functor(input, path.stem().string());
        }
    }else{
        std::cerr<<"must specify image_folder or image"<<std::endl;
    }
}

void test_croatia_general_recognizer()
{
    croatia_general_plate_recognizer cgpr;
    std::cout<<cgpr.fit("ZG7029M")<<std::endl;
    std::cout<<cgpr.fit("AB702M")<<std::endl;
    std::cout<<cgpr.fit("AB702ML")<<std::endl;
    std::cout<<cgpr.fit("HRZG7029M")<<std::endl;
    std::cout<<cgpr.fit("ZGM7029M")<<std::endl;
    std::cout<<cgpr.fit("ZGM7029ML")<<std::endl;
}

void test_grab_char(int argc, char **argv)
{
    auto const map =
            ocv::cmd::default_command_line_parser(argc, argv).first;
    morphology_localizer lpl;
    segment_character sc;
    //sc.set_show_debug_message(true);
    grab_character grab_char;
    test_algo(map, [&](cv::Mat const &input,
              std::string const &name)
    {
        lpl.localize_plate(input);
        auto const &contours = lpl.get_contours();
        for(size_t i = 0; i != contours.size(); ++i){
            sc.set_img_name(name + "_" + std::to_string(i));
            sc.detect_characters(lpl.get_resize_input(),
                                 contours[i]);
            if(sc.get_chars_contours().size() >= 5){
                grab_char.set_chars_name(name);
                grab_char.grab_chars(sc.get_bird_eyes_plate(),
                                     sc.get_chars_contours());
            }
        }
    });
}

void test_number_plate_localizer(int argc, char **argv)
{    
    auto const map =
            ocv::cmd::default_command_line_parser(argc, argv).first;
    morphology_localizer lpl;
    lpl.set_show_debug_message(true);
    test_algo(map, [&](cv::Mat const &input, std::string const&)
    {
        lpl.localize_plate(input);
    });
}

void test_prune_illegal_chars(int argc, char **argv)
{
    auto const map =
            ocv::cmd::default_command_line_parser(argc, argv).first;
    morphology_localizer lpl;
    segment_character sc;
    prune_illegal_chars plc;
    plc.set_show_debug_message(true);
    test_algo(map, [&](cv::Mat const &input, std::string const&)
    {
        lpl.localize_plate(input);
        for(auto const &contour : lpl.get_contours()){
            if(sc.detect_characters(lpl.get_resize_input(),
                                    contour)){
                plc.prune(sc.get_bird_eyes_plate(), 8,
                          sc.get_chars_contours());
            }else{
                std::cout<<"not a license plate"<<std::endl;
            }
        }
    });
}

void test_segment_character(int argc, char **argv)
{
    auto const map =
            ocv::cmd::default_command_line_parser(argc, argv).first;
    morphology_localizer lpl;
    lpl.set_show_debug_message(false);
    segment_character sc;
    sc.set_show_debug_message(true);
    test_algo(map, [&](cv::Mat const &input,
              std::string const &name)
    {
        lpl.localize_plate(input);
        auto const &contours = lpl.get_contours();
        for(size_t i = 0; i != contours.size(); ++i){
            sc.set_img_name(name + "_" + std::to_string(i));
            sc.detect_characters(lpl.get_resize_input(),
                                 contours[i]);            
        }
    });
}

void test_train_chars(int argc, char **argv)
{    
    auto const map =
            ocv::cmd::default_command_line_parser(argc, argv).first;
    if(map.count("image_folder") && map.count("output_folder")){
        train_chars tc(map["image_folder"].as<std::string>(),
                       map["output_folder"].as<std::string>());
        tc.train();
        tc.test_train_result();
    }else{
        std::cout<<"must speficy --image_folder and "
                   "--output_folder"<<std::endl;
    }//*/
}
