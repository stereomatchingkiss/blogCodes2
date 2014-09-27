#ifndef PROCESSTESTRESULT_HPP
#define PROCESSTESTRESULT_HPP

#include <string>
#include <vector>

/**
 * @brief process the test results
 */
class processPutGetTestResult
{
    template<typename T = double>
    using Vec = std::vector<T>;

    struct fileData
    {
        Vec<> elapse_;
        Vec<> frequency_;
        Vec<std::string> put_types_;
    };    

public:
    processPutGetTestResult();

    void average_time();
    void compare_two_performance_test_file();    

private:
    fileData get_data(std::string const &file_name);

private:
    fileData data_with_notify_;
    fileData data_without_notify_;
};

#endif // PROCESSTESTFILES_HPP
