#include "processtestResult.hpp"

int main()
{
    processPutGetTestResult process;
    //process.compare_two_performance_test_file();
    process.generate_change_log("log.txt");

    return 0;
}
