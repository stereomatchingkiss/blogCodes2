#include "noWork00.hpp"
#include "dispatch00.hpp"
#include "post00.hpp"

#include <functional>
#include <iostream>
#include <string>
#include <vector>

int main()
{
    std::vector<std::string> const Names{"dispatch example 00", "no work 00",
                                         "post example 00"};

    for(size_t i = 0; i != Names.size(); ++i){
        std::cout<<i<<" : "<<Names[i]<<std::endl;
    }

    size_t input = 0;
    std::cin>>input;
    if(input < Names.size()){
        std::vector<std::function<void()> > func{dispatch_example_00, no_work_00,
                    post_example_00};

        func[input]();
    }else{
        std::cout<<"out of range"<<std::endl;
    }

    return 0;
}
