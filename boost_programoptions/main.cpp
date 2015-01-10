#include "example_00.hpp"

#include <testFunction/testHelper.hpp>

#include <iostream>

int main(int argc, char const *argv[])
{
    /*for(int i = 0; i != argc; ++i){
        std::cout<<argv[i]<<std::endl;
    }*/

    basic_approach(argc, argv);
    //positional_options(argc, argv);
    //special_config(argc, argv);

    return 0;
}

