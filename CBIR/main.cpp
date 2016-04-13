#include "cbir_bovw.hpp"

#include <iostream>

int main()
{    
    try{
        cbir_bovw cb;
        cb.run();
    }catch(std::exception const &ex){
        std::cout<<ex.what()<<std::endl;
    }

    return 0;
}
