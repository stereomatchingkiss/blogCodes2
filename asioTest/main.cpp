#include "getLocalHostAddr.hpp"
#include "noWork00.hpp"
#include "dispatch00.hpp"
#include "post00.hpp"

#include <functional>
#include <iostream>
#include <string>
#include <vector>

int main()
<<<<<<< HEAD
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
=======
{    
    //dispatch_example_00();
    get_local_host_address();
    //no_work_00();
    //post_example_00();
>>>>>>> b2a494d1e66e3288af66a8ddfb2c1d84f94efe52

    return 0;
}
