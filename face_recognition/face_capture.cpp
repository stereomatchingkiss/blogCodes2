#include "face_capture.hpp"
#include "face_detector.hpp"

#include <boost/filesystem.hpp>

#include <boost/lexical_cast.hpp>

#include <boost/program_options.hpp>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <iostream>

face_capture::face_capture(std::string output_folder) :
    cap_(0),
    output_folder_(std::move(output_folder))
{

}

int face_capture::capture()
{
    if(check_validation() == 0){
        face_detector fd;
        while(true){
            if(!cap_.read(frame_)){
                break;
            }
            auto const &regions = fd.detect(frame_);
            frame_.copyTo(draw_);
            if(!regions.empty()){
                std::cout<<regions[0]<<std::endl;
                cv::rectangle(draw_, regions[0], {0,255,0}, 2);
            }
            cv::imshow("capture_mode",  draw_);
            int const key = cv::waitKey(30);
            if(key == 'q'){
                break;
            }else if(key == 'c'){
                if(!regions.empty()){
                    boost::uuids::uuid const uuid =
                            boost::uuids::random_generator()();
                    cv::imwrite(output_folder_ + "/" +
                                boost::lexical_cast<std::string>(uuid) + ".png",
                                frame_(regions[0]));
                }
            }
        }
    }

    return 0;
}

int face_capture::check_validation() const
{
    if(!boost::filesystem::exists(output_folder_)){
        if(!boost::filesystem::create_directory(output_folder_)){
            std::cout<<"cannot create output folder, please try again"
                    <<std::endl;
            return -1;
        }
    }

    if(!cap_.isOpened()){
        std::cout<<"cannot open camera"<<std::endl;
        return -1;
    }

    return 0;
}
