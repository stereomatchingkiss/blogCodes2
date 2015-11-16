#include "morphology_localizer.hpp"

#include <ocv_libs/cmd/command_prompt_utility.hpp>
#include <ocv_libs/file/utility.hpp>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

#include <iostream>

int main(int argc, char **argv)
{               
    //auto const image_name = "baza_slika/040603/P6040009.jpg";
    //auto const image_name = "baza_slika/280503/P5280115.jpg";
    std::string const Path = "baza_slika/test";
    auto images = ocv::file::get_directory_files(Path);
    morphology_localizer lpl;
    lpl.set_show_debug_message(true);
    //images.resize(1);
    //images[0] = "2014-Audi-R8-V10-Plus.jpg";    
    for(auto const &im_name : images){
        std::cout<<(Path + "/" + im_name)<<std::endl;
        cv::Mat input = cv::imread(Path + "/" + im_name);
        if(!input.empty()){                        
            std::vector<cv::Rect> plate_regions;
            lpl.localize_plate(input, plate_regions);
        }
    }//*/
}
