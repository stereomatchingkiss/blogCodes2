#include "feature_stitch.hpp"

#include <opencv2/calib3d.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/xfeatures2d.hpp>

#include <iostream>

feature_stitch::feature_stitch() :
    akaze_(cv::AKAZE::create())
{    
}

std::pair<cv::Mat, cv::Mat> feature_stitch::stitch_images(cv::Mat const &img1, cv::Mat const &img2,
                                                          stitch_direction direction,
                                                          double ratio, double reproj_thresh,
                                                          bool draw_matches)
{
    auto const kpts_desc1 = detect_and_compute(img2);
    auto const kpts_desc2 = detect_and_compute(img1);

    keypoints matches1, matches2;
    cv::Mat hmat;
    std::tie(matches1, matches2, hmat) =
            match_keypoints(kpts_desc1.first, kpts_desc2.first,
                            kpts_desc1.second, kpts_desc2.second,
                            ratio, reproj_thresh);

    if(!hmat.empty()){
        cv::Mat dst;
        cv::Size dsize;        
        if(direction == stitch_direction::horizontal){
            CV_Assert(img1.rows == img2.rows);

            dsize.width = img1.cols + img2.cols;
            dsize.height = img1.rows;
        }else{
            CV_Assert(img1.cols == img2.cols);

            dsize.height = img1.rows + img2.rows;
            dsize.width = img1.cols;            
        }

        cv::warpPerspective(img2, dst, hmat, dsize);
        img1.copyTo(dst(cv::Rect(0, 0, img2.cols, img2.rows)));

        if(draw_matches){
            return std::make_pair(std::move(dst),
                                  draw_matches_img(img1, img2, matches1, matches2, hmat));
        }

        return {std::move(dst), {}};
    }else{
        return {};
    }

}

std::tuple<feature_stitch::keypoints, feature_stitch::keypoints, cv::Mat>
feature_stitch::match_keypoints(feature_stitch::keypoints const &kpts1, feature_stitch::keypoints const &kpts2,
                                cv::Mat const &descriptor1, cv::Mat const &descriptor2,
                                double ratio, double reproj_thresh)
{
    using match_vec = std::vector<cv::DMatch>;

    //exhaustively compute euclidean distance between all feature vectors from both images
    //find the pairs of descriptors that have the smallest distance
    cv::BFMatcher matcher(cv::NORM_HAMMING);
    std::vector<match_vec> nn_matches;
    //top 2 matches because we need to apply David Lowe's ratio test
    matcher.knnMatch(descriptor1, descriptor2, nn_matches, 2);

    keypoints matches1, matches2;
    for(match_vec const &m : nn_matches){
        float const dist1 = m[0].distance;
        float const dist2 = m[1].distance;
        if(dist1 < ratio * dist2){
            matches1.emplace_back(kpts1[m[0].queryIdx]);
            matches2.emplace_back(kpts2[m[0].trainIdx]);
        }
    }    
    std::cout<<matches1[0].pt<<","<<matches2[0].pt<<std::endl;

    if(matches1.size() > 4){
        std::vector<cv::Point2f> points1, points2;
        for(size_t i = 0; i != matches1.size(); ++i){
            points1.emplace_back(matches1[i].pt);
            points2.emplace_back(matches2[i].pt);
        }
        cv::Mat hmat = cv::findHomography(points1, points2, cv::RANSAC, reproj_thresh);
        return std::make_tuple(std::move(matches1), std::move(matches2), std::move(hmat));
    }

    return {};
}

std::pair<feature_stitch::keypoints, cv::Mat> feature_stitch::detect_and_compute(const cv::Mat &img)
{
    CV_Assert(img.channels() == 1 || img.channels() == 3);
    keypoints kpts;
    cv::Mat descriptor;
    cv::Mat gray;
    if(img.channels() == 3){
        cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);
    }else{
        gray = img;
    }
    akaze_->detectAndCompute(gray, cv::noArray(), kpts, descriptor);

    return std::make_pair(std::move(kpts), std::move(descriptor));
}

cv::Mat feature_stitch::draw_matches_img(cv::Mat const &img1, cv::Mat const &img2,
                                         keypoints const &matches1,
                                         keypoints const &matches2,
                                         cv::Mat const &hmat)
{
    cv::Mat dst;
    keypoints inliers1, inliers2;
    std::vector<cv::DMatch> good_matches;
    std::tie(inliers1, inliers2, good_matches) = find_inliers(matches1, matches2, hmat);
    cv::drawMatches(img1, inliers1, img2, inliers2, good_matches, dst);

    return dst;
}

std::tuple<feature_stitch::keypoints, feature_stitch::keypoints, std::vector<cv::DMatch>>
feature_stitch::find_inliers(keypoints const &matches1,
                             keypoints const &matches2,
                             cv::Mat const &hmat)
{
    constexpr double inlier_threshold = 2.5;
    keypoints inliers1, inliers2;
    std::vector<cv::DMatch> good_matches;
    for(size_t i = 0; i != matches1.size(); ++i){        
        cv::Mat col = cv::Mat::ones(3,1, CV_64F);
        col.at<double>(0) = matches1[i].pt.x;
        col.at<double>(1) = matches1[i].pt.y;
        col *= hmat;
        col /= col.at<double>(2);        
        double const dist = std::sqrt(std::pow(col.at<double>(0) - matches2[i].pt.x, 2) +
                                      std::pow(col.at<double>(1) - matches2[i].pt.y, 2));
        if(dist < inlier_threshold){
            int const new_i = static_cast<int>(inliers1.size());
            inliers1.emplace_back(matches1[i]);
            inliers2.emplace_back(matches2[i]);
            good_matches.emplace_back(cv::DMatch(new_i, new_i, 0));
        }
    }

    return std::make_tuple(std::move(inliers1), std::move(inliers2), std::move(good_matches));
}
