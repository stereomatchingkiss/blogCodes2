#include "grab_character.hpp"

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <boost/filesystem.hpp>

#include <iomanip>
#include <iostream>
#include <sstream>

grab_character::grab_character() :
    chars_folder_(boost::filesystem::current_path().string() +
                  "/chars_folder")
{
    for(char i = '0'; i <= '9'; ++i){
        char_count_.insert({std::string(1,i), 0});
    }
    for(char i = 'a'; i <= 'z'; ++i){
        char_count_.insert({std::string(1,i), 0});
    }
    for(char i = 'A'; i <= 'Z'; ++i){
        char_count_.insert({std::string(1,i), 0});
    }
}

void grab_character::
grab_chars(cv::Mat const &plate,
           contours_type const &contours)
{
    if(!boost::filesystem::exists(chars_folder_)){
        boost::filesystem::create_directory(chars_folder_);
    }

    for(int i = 0; i != contours.size(); ++i){
        try{
            auto const rect = cv::boundingRect(contours[i]);
            cv::imshow("char candidate", plate(rect));
            int const key = cv::waitKey();
            if(key == '-'){
                continue;
            }else if(key == 'q'){
                break;
            }else{
                std::string const ckey(1, char(key));
                std::string const folder = chars_folder_ + "/" + ckey;
                if(!boost::filesystem::exists(folder)){
                    boost::filesystem::create_directory(folder);
                }
                std::ostringstream ostream;
                ostream<<folder<<"/"<<ckey<<"_"<<std::setfill('0')
                      <<std::setw(4)<<char_count_[ckey]++;
                cv::imwrite(ostream.str() + ".png",
                            plate(rect));
            }
        }catch(std::exception const &ex){
            std::cout<<ex.what()<<std::endl;
        }
    }//*/
}

void grab_character::set_chars_folder(const std::string &value)
{
    chars_folder_ = value;
}
