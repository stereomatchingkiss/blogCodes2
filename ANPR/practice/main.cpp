#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "detectRegions.hpp"

std::string const prefix("/Users/Qt/program/blogsCodes/pic/images/ANPR/");
std::string const names[] = {"9773BNB.jpg", "DSC_0562.jpg", "DSC_0566.jpg",
                             "2715DTZ.jpg", "3028BYS.JPG", "3154FFY.JPG",
                             "3266CNT.JPG", "3732FWW.JPG", "5445BSX.JPG",
                             "7215BGN.JPG", "9588DWV.jpg", "8995CCN.JPG"};
detectRegions region;

void showSingle(size_t index)
{
    cv::Mat input = cv::imread(prefix + names[index]);
    if(input.empty()){
        std::cerr<<"can't read image : "<<names[index]<<std::endl;
        return;
    }

    region.run(input);
}

void showAll()
{
    for(size_t i = 0; i != sizeof(names) / sizeof(std::string); ++i){
        showSingle(i);
    }
}

int main()
{    
    showAll();
    //showSingle(0);

    return 0;
}
