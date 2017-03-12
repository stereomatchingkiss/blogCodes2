#include "feature_extract_trainer.hpp"

#include <iostream>

int main(int argc, char *argv[])try
{
    if (argc < 2){
        std::cout << "Give the path to the setup file as the argument to this program" << std::endl;
        return -1;
    }

    feature_extract_trainer(argv[1]);

    return 0;
}catch(std::exception const &ex){
    std::cerr<<ex.what()<<std::endl;
}
