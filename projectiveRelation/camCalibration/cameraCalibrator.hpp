#ifndef CAMERACALIBRATOR_HPP
#define CAMERACALIBRATOR_HPP

#include <vector>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/highgui/highgui.hpp>

class cameraCalibrator
{
public:
    cameraCalibrator() : flag_(0), must_init_undistort_(true) {}

    int add_chess_board_points(std::vector<std::string> const &file_list, cv::Size const &board_size)
    {
        // the points on the chessboard
        std::vector<cv::Point2f> image_corners;
        std::vector<cv::Point3f> object_corners;

        // 3D Scene Points:
        // Initialize the chessboard corners
        // in the chessboard reference frame
        // The corners are at 3D location (X,Y,Z)= (i,j,0)
        for (int i = 0; i < board_size.height; ++i) {
            for (int j = 0; j < board_size.width; ++j) {
                object_corners.push_back(cv::Point3f(i, j, 0.0f));
            }
        }

        // 2D Image points:
        cv::Mat image; // to contain chessboard image
        int successes = 0;
        // for all viewpoints
        for (auto const &name : file_list) {

            // Open the image
            image = cv::imread(name, CV_LOAD_IMAGE_GRAYSCALE);

            // Get the chessboard corners
            bool found = cv::findChessboardCorners(
                        image, board_size, image_corners);

            // Get subpixel accuracy on the corners
            if(image_corners.size() == static_cast<size_t>(board_size.area())){
                cv::cornerSubPix(image, image_corners,
                                 cv::Size(5,5),
                                 cv::Size(-1,-1),
                                 cv::TermCriteria(cv::TermCriteria::MAX_ITER +
                                                  cv::TermCriteria::EPS,
                                                  30,                // max number of iterations
                                                  0.1));     // min accuracy
            }

            // If we have a good board, add it to our data
            if (image_corners.size() == static_cast<size_t>(board_size.area())) {

                // Add image and scene points from one view
                add_points(image_corners, object_corners);
                ++successes;
            }

            //Draw the corners
            //cv::drawChessboardCorners(image, board_size, image_corners, found);
            //cv::imshow("Corners on Chessboard", image);
            //cv::waitKey(100);
        }

        return successes;

    }

    // Add scene points and corresponding image points
    void add_points(const std::vector<cv::Point2f>& image_corners, const std::vector<cv::Point3f>& object_corners) {

        // 2D image points from one view
        image_points_.push_back(image_corners);
        // corresponding 3D scene points
        object_points_.push_back(object_corners);
    }

    // Calibrate the camera
    double calibrate(cv::Size const &image_size)
    {
        // undistorter must be reinitialized
        must_init_undistort_ = true;

        //Output rotations and translations
        std::vector<cv::Mat> rvecs, tvecs;

        // start calibration
        return
                cv::calibrateCamera(object_points_, // the 3D points
                                    image_points_,  // the image points
                                    image_size,    // image size
                                    camera_matrix_, // output camera matrix
                                    dist_coeffs_,   // output distortion matrix
                                    rvecs, tvecs, // Rs, Ts
                                    flag_);        // set options
        //                                      ,CV_CALIB_USE_INTRINSIC_GUESS);

    }

    // Getters
    cv::Mat get_camera_matrix() { return camera_matrix_; }
    cv::Mat get_dist_coeffs()   { return dist_coeffs_; }

    // Remove distortion in an image (after calibration)
    cv::Mat remap(cv::Mat const &image)
    {
        cv::Mat undistorted;
        if (must_init_undistort_) { // called once per calibration

            cv::initUndistortRectifyMap(
                        camera_matrix_,  // computed camera matrix
                        dist_coeffs_,    // computed distortion matrix
                        cv::Mat(),     // optional rectification (none)
                        cv::Mat(),     // camera matrix to generate undistorted
                        cv::Size(640,480),
                        //            image.size(),  // size of undistorted
                        CV_32FC1,      // type of output map
                        map1_, map2_);   // the x and y mapping functions

            must_init_undistort_ = false;
        }

        // Apply mapping functions
        cv::remap(image, undistorted, map1_, map2_,
                  cv::INTER_LINEAR); // interpolation type

        return undistorted;

    }

    // Set the calibration flag
    void setCalibrationFlag(bool radial8CoeffEnabled = false, bool tangentialParamEnabled = false)
    {
        // Set the flag used in cv::calibrateCamera()
        flag_ = 0;
        if (!tangentialParamEnabled) flag_ += CV_CALIB_ZERO_TANGENT_DIST;
        if (radial8CoeffEnabled) flag_ += CV_CALIB_RATIONAL_MODEL;
    }

private:
    std::vector<std::vector<cv::Point3f> > object_points_;
    std::vector<std::vector<cv::Point2f> > image_points_;

    cv::Mat camera_matrix_;
    cv::Mat dist_coeffs_;
    int flag_;
    cv::Mat map1_, map2_;
    bool must_init_undistort_;
};

#endif // CAMERACALIBRATOR_HPP
