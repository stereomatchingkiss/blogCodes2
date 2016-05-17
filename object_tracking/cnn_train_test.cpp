#include "cnn_train_test.hpp"
#include "read_stanford40_pose.hpp"
#include "read_usc_pedestrian.hpp"

#include <opencv2/highgui.hpp>

#include <iostream>

cnn_train_test::cnn_train_test()
{
    read_file();
}

void cnn_train_test::read_file()
{
    read_stanford40_pose rsp;
    rsp.parse_file("C:/Users/yyyy/Qt/computer_vision_dataset/"
                   "human_pose/Stanford40/XMLAnnotations/"
                   "applauding_022.xml");
    auto maps =
            rsp.parse_folder("C:/Users/yyyy/Qt/computer_vision_dataset/"
                             "human_pose/Stanford40");
    auto vecs = maps["applauding_022.jpg"];
    std::cout<<vecs[0]<<std::endl;
    /*read_usc_pedestrian rup;
    auto mats =
            rup.parse_folder(
    {"C:/Users/yyyy/Qt/computer_vision_dataset/pedestrian/USCPedestrianSetC"});
    std::cout<<"mats size "<<mats.size()<<std::endl;//*/
}
