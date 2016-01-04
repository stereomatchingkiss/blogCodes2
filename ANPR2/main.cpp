#include "fhog_trainer.hpp"
#include "morphology_localizer.hpp"
#include "prune_illegal_chars.hpp"
#include "segment_character.hpp"

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

void test_number_plate_localizer(int argc, char **argv);
void test_prune_illegal_chars(int argc, char **argv);
void test_segment_character(int argc, char **argv);

void test_four_points_transform();

int main(int argc, char **argv)
{                   
    //fhog_number_plate_trainer fhog_trainer(argc, argv);
    //test_number_plate_localizer(argc, argv);
    //test_prune_illegal_chars(argc, argv);
    test_segment_character(argc, argv);
    //test_four_points_transform();
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

void test_four_points_transform()
{
    std::map<std::string, std::vector<cv::Point2f>> mapper
    {
        {
            "get-perspective-transform-example/images/example_01.png",
            {
                {73, 239}, {356, 117},
                {475, 265}, {187, 443}
            }
        },
        {
            "get-perspective-transform-example/images/example_02.png",
            {
                {101, 185}, {393, 151},
                {479, 323}, {187, 441}
            }
        },
        {
            "get-perspective-transform-example/images/example_03.png",
            {
                {63, 242}, {291, 110},
                {361, 252}, {78, 386}
            }
        }
    };
    std::string const img_name = "get-perspective-transform-example/"
                                 "images/example_03.png";
    auto input = cv::imread(img_name);
    if(!input.empty()){
        cv::Point2f points[4];
        std::copy(std::begin(mapper[img_name]), std::end(mapper[img_name]),
                  std::begin(points));
        std::cout<<"before sort"<<std::endl;
        for(auto const pt : points){
            std::cout<<pt<<std::endl;
        }
        ocv::sort_corners(points, std::begin(points));
        //auto const center = ocv::corners_center(points);
        //std::cout<<"center : "<<center<<std::endl;
        //cv::circle(input, center, 3, {255,0,0}, 2);
        for(size_t i = 0; i != 4; ++i){
            std::cout<<i<<" : "<<points[i]<<std::endl;
            double const scale = 1.0;
            int const thickness = 2;
            auto text_point = points[i];
            text_point.y -= 10;
            cv::putText(input, std::to_string(i), text_point,
                        cv::FONT_HERSHEY_COMPLEX, scale, {255,0,0}, thickness);
            cv::circle(input, points[i], 3, {0,255,0}, 2);
        }
        auto warp_input = ocv::four_points_transform(input, points);
        cv::imshow("input", input);
        cv::imshow("warp", warp_input);
        cv::waitKey();
    }
}
