#include "update_image_label.hpp"

#include <boost/program_options.hpp>

#include <iostream>

int main(int argc, char **argv)
{
    try{
        using namespace boost::program_options;

        options_description desc{"option"};
        desc.add_options()
                ("help,h", "Help menu")
                ("input_file,i", value<std::string>()->required(),
                 "xml file to update")
                ("output_file,o", value<std::string>()->required(),
                 "The xml file after update")
                ("ratio,r", value<double>()->default_value(1.0),
                 "Ratio multiply on the box value");

        variables_map vm;
        store(parse_command_line(argc, argv, desc), vm);

        if(vm.count("help")){
            std::cerr<<desc<<std::endl;
            return 0;
        }
        notify(vm);

        update_image_label uil(vm["input_file"].as<std::string>(),
                vm["output_file"].as<std::string>());
        uil.update_box_ratio_of_label(vm["ratio"].as<double>());
    }catch(std::exception const &ex){
        std::cerr<<ex.what()<<std::endl;
    }
}
