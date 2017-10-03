#include "hough_lines_polygon.hpp"
#include "mapping_files.hpp"
#include "marble_slab.hpp"
#include "so_obj_detect_00.hpp"

#include <exception>
#include <iostream>

int main()try
{
    try{
        //hough_lines_polygon_detection();
        //mapping_files();
        marble_slab();
        //identify_objects_sequentially();
    }catch(std::exception const &ex){
        std::cerr<<ex.what()<<"\n";
    }
}catch(std::exception const &ex){
    std::cerr<<ex.what()<<std::endl;
}
