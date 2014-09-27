#ifndef PARSESVNLOG_HPP
#define PARSESVNLOG_HPP

#include <string>
#include <vector>

class parseSVNLog
{
public:
    struct logStructure
    {
        logStructure();

        size_t revision_; //by now this contains lot of message, temp solution
        std::string branch_;
        size_t commit_year_;
        size_t commit_month_;
        size_t commit_day_;
        std::vector<std::string> commit_files_;
        std::string commit_user_;
        //std::string commit_month_;
        //size_t commit_day_;
        //size_t commit_year_;

        //std::vector<std::string> commit_comments_;
    };

    parseSVNLog();

    std::vector<parseSVNLog::logStructure>
    parse_logs(std::string const &file_name) const;

private:
    std::string read_whole_file(std::string const &file_name) const;
};


std::ostream& operator<<(std::ostream &out,
                         parseSVNLog::logStructure const &log);

#endif // PARSESVNLOG_HPP
