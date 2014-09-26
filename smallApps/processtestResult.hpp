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

    struct logStructure
    {
        std::string dashs_;
        std::string revision_;//by now this contains lot of message, temp solution
        std::string change_path_flag_;
        std::vector<std::string> commit_files_;
        std::string commit_month_;
        size_t commit_day_;
        size_t commit_year_;
        std::string user_name_;
        std::vector<std::string> commit_messages_;
    };

public:
    processPutGetTestResult();

    void average_time();
    void compare_two_performance_test_file();
    void generate_change_log(std::string const &log_name);

    void proces_change_log(logStructure const &log, std::ofstream &out);
    logStructure read_change_log(std::ifstream &in);

private:
    fileData get_data(std::string const &file_name);

private:
    fileData data_with_notify_;
    fileData data_without_notify_;
};

#endif // PROCESSTESTFILES_HPP
