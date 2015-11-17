#include <boost/program_options.hpp>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>

int main(int argc, char **argv)
{               
    using namespace boost::program_options;

    try{
        options_description desc{"Options"};
        desc.add_options()
                ("help,h", "Help screen")
                ("file,f", value<std::string>()->required(), "File to process");

        variables_map vm;
        store(parse_command_line(argc, argv, desc), vm);
        notify(vm);

        std::ifstream in(vm["file"].as<std::string>());
        std::string buffer;
        std::getline(in, buffer);

        constexpr size_t attribute_size = 7;

        std::vector<std::vector<double>> attr_vec;
        while(std::getline(in, buffer)){
            char symbol = '\0';
            std::istringstream istr(buffer);
            attr_vec.emplace_back(std::vector<double>(attribute_size));
            for(size_t i = 0; i != attribute_size - 1; ++i){
                istr>>attr_vec.back()[i]>>symbol;
            }
            istr>>attr_vec.back()[attribute_size - 1];
        }

        std::string const attr_name[] = {"CArea","BArea","Perimeter","Aspect",
                                         "Extent","Solidity","PolySize"};
        for(size_t i = 0; i != attribute_size; ++i){
            auto const min_max =
                    std::minmax_element(std::begin(attr_vec), std::end(attr_vec),
                                        [=](auto const &lhs, auto const &rhs)
            {
                return lhs[i] < rhs[i];
            });
            std::cout<<"range of "<<attr_name[i]<<" : ["
                    <<(*min_max.first)[i]<<","<<(*min_max.second)[i]<<"]"
                   <<std::endl;;
        }
    }catch(std::exception const &ex){
        std::cerr<<ex.what()<<std::endl;
    }
}
