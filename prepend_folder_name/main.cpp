#include <ocv_libs/cmd/command_prompt_utility.hpp>
#include <ocv_libs/file/utility.hpp>

#include <fstream>
#include <iostream>

int main(int argc, char *argv[])
{
    using namespace ocv;

    try{
        auto map = cmd::default_command_line_parser(argc, argv).first;
        if(map.count("image_folder") && map.count("output_folder")){
            std::ofstream out("locations.txt");
            auto const folder = map["image_folder"].as<std::string>();
            auto const out_folder = map["output_folder"].as<std::string>();
            if(!boost::filesystem::exists(out_folder)){
                boost::filesystem::create_directories(out_folder);
            }
            auto const folders = file::get_directory_folders(folder);
            //std::cout<<"folders size "<<folders.size()<<std::endl;
            for(auto const &fo_name : folders){
                auto const files = file::get_directory_files(folder + "/" + fo_name);
                //std::cout<<"files size "<<files.size()<<std::endl;
                auto const current_folder = folder+ "/" + fo_name;
                auto const new_folder =
                        boost::filesystem::current_path().generic_string() +
                        "/" + out_folder + "/" + fo_name;
                for(auto const &fname : files){
                    std::string const new_name = new_folder +
                            "_" + fname;
                    //std::cout<<current_folder<<std::endl;
                    //std::cout<<new_name<<std::endl;
                    //std::cin.get();
                    boost::filesystem::copy_file(current_folder + "/" + fname,
                                                 new_name,
                                                 boost::filesystem::copy_option::overwrite_if_exists);
                    out<<new_name<<"\n";
                }
            }
        }else{
            std::cout<<"must specify image_folder and output_folder"<<std::endl;
            return -1;
        }
    }catch(std::exception const &ex){
        std::cout<<ex.what()<<std::endl;
    }

    return 0;
}
