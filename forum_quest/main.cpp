#include "hough_lines_polygon.hpp"
#include "mapping_files.hpp"
#include "so_obj_detect_00.hpp"

#include <exception>
#include <iostream>

int main()
{
    try{
        //hough_lines_polygon_detection();
        mapping_files();
        //identify_objects_sequentially();
    }catch(std::exception const &ex){
        std::cerr<<ex.what()<<"\n";
    }
}
