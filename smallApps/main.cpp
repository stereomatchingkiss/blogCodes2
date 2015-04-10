#include "appStatistic.hpp"
#include "cpp_programming_lg_4.h"
#include "generateNewSVNLog.hpp"
#include "parseSVNLog.hpp"
#include "svnLogStructure.hpp"

#include <boost/container/vector.hpp>

#include <boost/fusion/adapted/boost_tuple.hpp>
#include <boost/fusion/algorithm.hpp>

#include <exception>
#include <iomanip>
#include <iostream>

int main(int argc, char const *argv[])
{
    try{
        //generate_changes_log(argc, argv);
        reference_deduction_23_5_2();

        //auto value = boost::make_tuple(3, 3.44, false);

        //boost::fusion::for_each(value, tuple_func());

        /*if(argc > 2){
            cpu_usage_statistic(argv[1]);
        }else if(argc > 1){
            app_statistic(argv[1]);
        }*/

    }catch(std::exception const &ex){
        std::cerr<<ex.what()<<std::endl;
    }

    return 0;
}
