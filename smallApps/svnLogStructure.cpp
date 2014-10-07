#include "svnLogStructure.hpp"

#include <ostream>

svnLogStructure::svnLogStructure() :
    revision_{0}
{}


hh_mm_ss::hh_mm_ss() :
    hh_{0}, mm_{0}, ss_{0}
{}

yy_mm_dd::yy_mm_dd() :
    yy_{0},
    mm_{0},
    dd_{0}
{}

std::ostream &operator<<(std::ostream &out, const svnLogStructure &log)
{
    out<<log.revision_<<", "<<log.branch_<<std::endl;
    //out<<log.commit_year_<<"-"<<log.commit_month_<<"-";
    //out<<log.commit_day_<<", ";
    //out<<log.hh_mm_ss_[0]<<":"<<log.hh_mm_ss_[1];
    //out<<":"<<log.hh_mm_ss_[2]<<std::endl;
    out<<log.yy_mm_dd_.yy_<<"-"<<log.yy_mm_dd_.mm_<<"-";
    out<<log.yy_mm_dd_.dd_<<", ";
    out<<log.hh_mm_ss_.hh_<<":"<<log.hh_mm_ss_.mm_;
    out<<":"<<log.hh_mm_ss_.ss_<<std::endl;
    for(auto const &Str : log.commit_files_){
        out<<Str<<std::endl;
    }
    //out<<log.commit_user_<<std::endl;

    for(auto const &Str : log.commit_comments_){
        out<<Str<<std::endl;
    }

    return out;
}

