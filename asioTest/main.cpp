#include "getLocalHostAddr.hpp"
#include "noWork00.hpp"
#include "dispatch00.hpp"
#include "post00.hpp"
#include "timer00.hpp"

#include <functional>
#include <iostream>
#include <string>
#include <vector>

int main()
{
    std::vector<std::string> const Names{"dispatch example 00", "no work 00",
                                         "post example 00", "timer example 00",
                                         "getLocalHostAddr"};

    for(size_t i = 0; i != Names.size(); ++i){
        std::cout<<i<<" : "<<Names[i]<<std::endl;
    }

    size_t input = 0;
    std::cin>>input;
    if(input < Names.size()){
        std::vector<std::function<void()>> func{
                                           dispatch_example_00, no_work_00,
                                           post_example_00, timer_00,
                                           get_local_host_address
    };

        func[input]();
    }else{
        std::cout<<"out of range"<<std::endl;
    }
    std::cout<<"program exist"<<std::endl;
    std::cin.get();
    std::cin.get();

    return 0;
}
