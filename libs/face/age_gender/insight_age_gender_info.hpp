#ifndef OCV_FACE_INSIGHT_AGE_GENDER_INFO_HPP
#define OCV_FACE_INSIGHT_AGE_GENDER_INFO_HPP

#include "gender_info.hpp"

namespace ocv{

namespace face{

struct insight_age_gender_info
{
    int age_ = 0;
    gender_info gender_ = gender_info::male_;
};

}

}

#endif // OCV_FACE_INSIGHT_AGE_GENDER_INFO_HPP
