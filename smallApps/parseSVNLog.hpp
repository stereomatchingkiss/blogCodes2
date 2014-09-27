#ifndef PARSESVNLOG_HPP
#define PARSESVNLOG_HPP

#include <string>
#include <vector>

class parseSVNLog
{
public:
    struct hh_mm_ss
    {
        hh_mm_ss();

        size_t hh_;
        size_t mm_;
        size_t ss_;
    };

    struct yy_mm_dd
    {
        yy_mm_dd();

        size_t yy_;
        size_t mm_;
        size_t dd_;
    };

    struct logStructure
    {
        logStructure();

        size_t revision_; //by now this contains lot of message, temp solution
        std::string branch_;
        yy_mm_dd yy_mm_dd_;
        hh_mm_ss hh_mm_ss_;
        std::vector<std::string> commit_files_;
        std::string commit_user_;
        std::vector<std::string> commit_comments_;
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
