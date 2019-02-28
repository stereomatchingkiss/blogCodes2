#ifndef HEAD_POSE_ESTIMATION_HPP
#define HEAD_POSE_ESTIMATION_HPP

#include <dlib/image_processing.h>

#include <opencv2/core.hpp>

#include <array>

namespace ocv
{

namespace face{

struct head_pose_info;

class head_pose_estimation
{
public:
    struct result_type
    {
        double pitch_ = 0;
        double roll_ = 0;
        double yaw_ = 0;
    };

    head_pose_estimation();

    head_pose_info estimate(dlib::full_object_detection const &shape, cv::Size const &img_size);

private:
    struct quaterniond
    {
        double w,x,y,z;
    };

    void create_2d_image_points(dlib::full_object_detection const &shape);
    std::vector<cv::Point3d> create_3d_model_points() const;
    cv::Mat create_camera_matrix(double focal_length, cv::Point2d const &center) const;

    void quaterniond_to_euler_angle(quaterniond& q, double& roll, double& yaw, double& pitch) const;

    double radian_to_degrees(double input) const;

    cv::Mat dist_coeffs_;
    std::vector<cv::Point2d> image_points_;
    std::vector<cv::Point3d> model_points_;
};

}

}

#endif // HEAD_POSE_ESTIMATION_HPP
