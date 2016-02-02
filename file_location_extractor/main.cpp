#include <ocv_libs/file/utility.hpp>

#include <boost/program_options.hpp>

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace boost::program_options;

variables_map parse_command_line(int argc, char *argv[]);

int main(int argc, char *argv[])
{
    try{
        auto map = parse_command_line(argc, argv);
        if(map.count("help")){
            return 0;
        }

        using namespace ocv::file;

        auto const input_folder = map["input_folder"].as<std::string>();
        auto const output_file = map["output_file"].as<std::string>();
        auto result = get_directory_path(input_folder, true);
        std::ofstream out(output_file);
        for(auto const &path : result){
            if(is_regular_file(path)){
                out<<absolute(path).generic_string()<<"\n";
            }
        }
    }catch(std::exception const &ex){
        std::cout<<ex.what()<<std::endl;
    }

    return 0;
}

variables_map parse_command_line(int argc, char *argv[])
{
    options_description desc{"Options"};
    desc.add_options()
            ("help,h", "Help screen")
            ("input_folder,i", value<std::string>()->required(), "Specify the folder to process")
            ("output_file,o", value<std::string>()->required(), "Specify the name of the output file");

    variables_map vm;
    store(parse_command_line(argc, argv, desc), vm);

    if(vm.count("help")){
        std::cout << desc << std::endl;
        return vm;
    }

    notify(vm);

    return vm;
}
