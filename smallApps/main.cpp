#include "parseSVNLog.hpp"
#include "processtestResult.hpp"

#include <exception>
#include <iostream>

int main()
{
    try{
        parseSVNLog log;
        auto const Logs = log.parse_logs("log.txt");
        for(auto const &Log : Logs){
            std::cout<<Log<<std::endl;
        }

        //processPutGetTestResult process;
        //process.compare_two_performance_test_file();
        //process.generate_change_log("log.txt");
    }catch(std::exception const &ex){
        std::cerr<<ex.what()<<std::endl;
    }

    return 0;
}
