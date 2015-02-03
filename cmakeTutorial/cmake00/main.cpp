#include <chrono>
#include <iostream>
#include <string>

#include <boost/algorithm/string.hpp>
#include <boost/thread.hpp>

void print()
{
    std::cout<<"wahaha"<<std::endl;
}

int main()
{       
    std::string temp{"kilala"};
    boost::replace_first(temp, "ki", "lala");
    std::cout<<temp<<std::endl;

    boost::thread thr(print);
    thr.join();

    return 0;
}

