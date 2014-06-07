#include "moveLibsAndDLL.hpp"

#include <QStringList>

#include <iostream>
#include <limits>

int main(int argc, char *argv[])
{        
    if(argc > 1){
        QStringList filter;
        for(int i = 1; i < argc; ++i){
            filter.push_back(argv[i]);
        }
        move_libs_and_dll(filter);
    }else{
        move_libs_and_dll(QStringList()<<"*.dll"<<"*.lib");
    }

    return 0;
}

