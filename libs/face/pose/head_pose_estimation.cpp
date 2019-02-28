#include "head_pose_estimation.hpp"

#include "head_pose_info.hpp"

#include <opencv2/calib3d.hpp>
#include <opencv2/imgproc.hpp>

namespace ocv
{

namespace face{

head_pose_estimation::head_pose_estimation() :
    image_points_(6),
    model_points_(create_3d_model_points())
{
    dist_coeffs_ = cv::Mat::zeros(4, 1, cv::DataType<double>::type);
}

head_pose_info head_pose_estimation::estimate(const dlib::full_object_detection &shape, const cv::Size &img_size)
{
    cv::Mat const camera_matrix = create_camera_matrix(img_size.width,
                                                       cv::Point2d(img_size.width/2, img_size.height/2));

    cv::Mat rotation_vector;
    cv::Mat translation_vector;

    create_2d_image_points(shape);
    cv::solvePnP(model_points_, image_points_,
                 camera_matrix, dist_coeffs_,
                 rotation_vector, translation_vector);

    //calculate rotation angles
    double const theta = cv::norm(rotation_vector, CV_L2);

    //transformed to quaterniond
    quaterniond q;
    q.w_ = cos(theta / 2);
    q.x_ = sin(theta / 2)*rotation_vector.at<double>(0, 0) / theta;
    q.y_ = sin(theta / 2)*rotation_vector.at<double>(0, 1) / theta;
    q.z_ = sin(theta / 2)*rotation_vector.at<double>(0, 2) / theta;

    return quaterniond_to_euler_angle(q);
}

void head_pose_estimation::create_2d_image_points(const dlib::full_object_detection &shape)
{
    image_points_[0] = cv::Point2d(shape.part(30).x(), shape.part(30).y());  // Nose tip
    image_points_[1] = cv::Point2d(shape.part(8).x(), shape.part(8).y());    // Chin
    image_points_[2] = cv::Point2d(shape.part(36).x(), shape.part(36).y());  // Left eye left corner
    image_points_[3] = cv::Point2d(shape.part(45).x(), shape.part(45).y());  // Right eye right corner
    image_points_[4] = cv::Point2d(shape.part(48).x(), shape.part(48).y());  // Left Mouth corner
    image_points_[5] = cv::Point2d(shape.part(54).x(), shape.part(54).y());  // Right mouth corner
}

std::vector<cv::Point3d> head_pose_estimation::create_3d_model_points() const
{
    // 3D model points.
    std::vector<cv::Point3d> model_points;
    model_points.emplace_back(0.0, 0.0, 0.0);               // Nose tip
    model_points.emplace_back(0.0, -330.0, -65.0);          // Chin
    model_points.emplace_back(-225.0, 170.0, -135.0);       // Left eye left corner
    model_points.emplace_back(225.0, 170.0, -135.0);        // Right eye right corner
    model_points.emplace_back(-150.0, -150.0, -125.0);      // Left Mouth corner
    model_points.emplace_back(150.0, -150.0, -125.0);       // Right mouth corner

    return model_points;
}

cv::Mat head_pose_estimation::create_camera_matrix(double focal_length, const cv::Point2d &center) const
{
    return  cv::Mat_<double>(3,3) <<
            focal_length, 0,            center.x,
            0,            focal_length, center.y,
            0,            0,            1;
}

head_pose_info head_pose_estimation::quaterniond_to_euler_angle(head_pose_estimation::quaterniond const &q) const
{
    double const ysqr = q.y_ * q.y_;
    head_pose_info result;
    // pitch (x-axis rotation)
    double const t0 = +2.0 * (q.w_ * q.x_ + q.y_ * q.z_);
    double const t1 = +1.0 - 2.0 * (q.x_ * q.x_ + ysqr);
    result.pitch_ = radian_to_degrees(std::atan2(t0, t1));

    // yaw (y-axis rotation)
    double t2 = +2.0 * (q.w_ * q.y_ - q.z_ * q.x_);
    t2 = t2 > 1.0 ? 1.0 : t2;
    t2 = t2 < -1.0 ? -1.0 : t2;
    result.yaw_ = radian_to_degrees(std::asin(t2));

    // roll (z-axis rotation)
    double const t3 = +2.0 * (q.w_ * q.z_ + q.x_ * q.y_);
    double const t4 = +1.0 - 2.0 * (ysqr + q.z_ * q.z_);
    result.roll_ = radian_to_degrees(std::atan2(t3, t4));

    return result;
}

double head_pose_estimation::radian_to_degrees(double input) const
{
    return input * 180 / 3.141592653589793;
}

}

}
