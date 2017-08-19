#ifndef FEATURES_STITCHING_HPP
#define FEATURES_STITCHING_HPP

#include "utility.hpp"

#include <opencv2/core.hpp>

#include <tuple>

namespace cv {
class AKAZE;
}

/**
 * @brief Encapsulate stitching algorithm based on
 * features matching
 */
class feature_stitch
{
public:
    feature_stitch();

    /**
     * stitch img1 and img2 together
     * @param img1 self explained
     * @param img2 self explained
     * @param direction determine stitching direction, it could be horizontal nor
     * vertical. horizontal required height of img1 same as img2, vertical
     * required width of img1 same as img2
     * @param ratio ratio of David Lowe's ratio test
     * @param reproj_thresh reproject threshold of RANSAC algorithm
     * @param draw_matches true will return visualize keypoints correspondences between two images
     * @return First, stitched image. Second, visualize keypoints
     */
    std::pair<cv::Mat, cv::Mat> stitch_images(cv::Mat const &img1, cv::Mat const &img2,
                                              stitch_direction direction,
                                              double ratio = 0.75, double reproj_thresh = 4.0,
                                              bool draw_matches = false);

private:
    using keypoints = std::vector<cv::KeyPoint>;

    std::pair<keypoints, cv::Mat> detect_and_compute(cv::Mat const &img);
    cv::Mat draw_matches_img(cv::Mat const &img1, cv::Mat const &img2,
                             keypoints const &matches1, keypoints const &matches2,
                             cv::Mat const &hmat);

    std::tuple<keypoints, keypoints, std::vector<cv::DMatch>>
    find_inliers(keypoints const &matches1, keypoints const &matches2,
                 cv::Mat const &hmat);

    std::tuple<feature_stitch::keypoints, feature_stitch::keypoints, cv::Mat>
    match_keypoints(keypoints const &kpts1, keypoints const &kpts2,
                    cv::Mat const &descriptor1, cv::Mat const &descriptor2,
                    double ratio = 0.75, double reproj_thresh = 4.0);

    cv::Ptr<cv::AKAZE> akaze_;

};

#endif // FEATURES_STITCHING_HPP
