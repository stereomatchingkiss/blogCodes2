#ifndef OCV_FACE_HEAD_POSE_INFO_HPP
#define OCV_FACE_HEAD_POSE_INFO_HPP

#include <opencv2/core.hpp>

namespace ocv
{

namespace face{

struct head_pose_info
{
    double pitch_ = 0;
    double roll_ = 0;
    double yaw_ = 0;
};

}

}

#endif // OCV_FACE_HEAD_POSE_INFO_HPP
