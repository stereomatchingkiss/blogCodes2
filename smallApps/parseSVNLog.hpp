#ifndef PARSESVNLOG_HPP
#define PARSESVNLOG_HPP

#include "svnLogStructure.hpp"

#include <string>
#include <vector>

class parseSVNLog
{
public:
    parseSVNLog();

    std::vector<svnLogStructure>
    parse_logs(std::string const &file_name) const;

private:
    std::string read_whole_file(std::string const &file_name) const;
};

#endif // PARSESVNLOG_HPP
