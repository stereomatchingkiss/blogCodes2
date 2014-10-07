#ifndef SVNLOGSTRUCTURE_HPP
#define SVNLOGSTRUCTURE_HPP

#include <boost/fusion/include/adapt_struct.hpp>

#include <string>
#include <vector>

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

struct svnLogStructure
{
    svnLogStructure();

    size_t revision_;
    std::string branch_;
    yy_mm_dd yy_mm_dd_;
    hh_mm_ss hh_mm_ss_;
    std::vector<std::string> commit_files_;    
    std::vector<std::string> commit_comments_;
};

BOOST_FUSION_ADAPT_STRUCT(
        hh_mm_ss,
        (size_t, hh_)
        (size_t, mm_)
        (size_t, ss_)
        )

BOOST_FUSION_ADAPT_STRUCT(
        yy_mm_dd,
        (size_t, yy_)
        (size_t, mm_)
        (size_t, dd_)
        )

BOOST_FUSION_ADAPT_STRUCT(
        svnLogStructure,
        (size_t, revision_)
        (std::string, branch_)
        (yy_mm_dd, yy_mm_dd_)
        (hh_mm_ss, hh_mm_ss_)
        (std::vector<std::string>, commit_files_)        
        (std::vector<std::string>, commit_comments_)
        )

std::ostream& operator<<(std::ostream &out,
                         svnLogStructure const &log);

#endif // SVNLOGSTRUCTURE_HPP
