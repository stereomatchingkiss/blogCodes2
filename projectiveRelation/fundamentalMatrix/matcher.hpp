#ifndef MATCHER_HPP
#define MATCHER_HPP

#include <algorithm>
#include <vector>

#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/nonfree/nonfree.hpp>

class RobustMatcher {

private:

    // pointer to the feature point detector object
    cv::Ptr<cv::Feature2D> featureAlgo;
    float ratio; // max ratio between 1st and 2nd NN
    bool refineF; // if true will refine the F matrix
    double distance; // min distance to epipolar
    double confidence; // confidence level (probability)

public:

    RobustMatcher() : featureAlgo(new cv::SURF(10)),
        ratio(0.65f), refineF(true), distance(3.0), confidence(0.99)
    {
    }

    // Set the feature detector
    void setFeatureAlgo(cv::Ptr<cv::FeatureDetector>& detect) {

        featureAlgo = detect;
    }

    // Set the minimum distance to epipolar in RANSAC
    void setMinDistanceToEpipolar(double d) {

        distance = d;
    }

    // Set confidence level in RANSAC
    void setConfidenceLevel(double c) {

        confidence = c;
    }

    // Set the NN ratio
    void setRatio(float r) {

        ratio = r;
    }

    // if you want the F matrix to be recalculated
    void refineFundamental(bool flag) {

        refineF = flag;
    }

    // Clear matches for which NN ratio is > than threshold
    // return the number of removed points
    // (corresponding entries being cleared, i.e. size will be 0)
    int ratioTest(std::vector<std::vector<cv::DMatch>>& matches) {

        int removed = 0;
        /*auto it = std::partition(std::begin(matches), std::end(matches), [&](cv::DMatch const &data)
        {
            // if 2 NN has been identified
            if (data.size() > 1) {
                // check distance ratio
                if (data[0].distance / data[1].distance > ratio) {
                    ++removed;
                    return true;
                }
            } else { // does not have 2 neighbours
                ++removed;
                return true;
            }
        });
        matches.erase(it, std::end(matches));*/

        // for all matches
        for (auto &data : matches){
            // if 2 NN has been identified
            if (data.size() > 1) {
                // check distance ratio
                if (data[0].distance / data[1].distance > ratio) {
                    data.clear(); // remove match
                    ++removed;
                }
            } else { // does not have 2 neighbours
                data.clear(); // remove match
                ++removed;
            }
        }

        return removed;
    }

    // Insert symmetrical matches in symMatches vector
    void symmetryTest(std::vector<std::vector<cv::DMatch>> const &matches1,
                      std::vector<std::vector<cv::DMatch>> const &matches2,
                      std::vector<cv::DMatch>& symMatches) {

        // for all matches image 1 -> image 2
        for(auto const &data1 : matches1){

            if (data1.size() < 2) // ignore deleted matches
                continue;

            // for all matches image 2 -> image 1
            for(auto const &data2 : matches2){
                if (data2.size() < 2) // ignore deleted matches
                    continue;
                // Match symmetry test
                if (data1[0].queryIdx == data2[0].trainIdx  &&
                        data2[0].queryIdx == data1[0].trainIdx) {
                    // add symmetrical match
                    symMatches.emplace_back(data1[0].queryIdx,
                                            data1[0].trainIdx,
                                            data1[0].distance);
                    break; // next match in image 1 -> image 2
                }
            }
        }
    }

    // Identify good matches using RANSAC
    // Return fundemental matrix
    cv::Mat ransacTest(const std::vector<cv::DMatch>& matches,
                       const std::vector<cv::KeyPoint>& keypoints1,
                       const std::vector<cv::KeyPoint>& keypoints2,
                       std::vector<cv::DMatch>& outMatches) {

        // Convert keypoints into Point2f
        std::vector<cv::Point2f> points1, points2;
        for (auto const &data :  matches){
            // Get the position of left keypoints
            points1.emplace_back(keypoints1[data.queryIdx].pt);
            // Get the position of right keypoints
            points2.emplace_back(keypoints2[data.trainIdx].pt);
        }

        // Compute F matrix using RANSAC
        size_t const matchesSize = matches.size();
        std::vector<uchar> inliers(matchesSize, 0);
        cv::Mat fundemental = cv::findFundamentalMat(
                    cv::Mat(points1),cv::Mat(points2), // matching points
                    inliers,      // match status (inlier ou outlier)
                    CV_FM_RANSAC, // RANSAC method
                    distance,     // distance to epipolar line
                    confidence);  // confidence probability

        // extract the surviving (inliers) matches
        // for all matches
        for (size_t i = 0; i != matchesSize; ++i) {
            if (inliers[i]) { // it is a valid match
                outMatches.emplace_back(matches[i]);
            }
        }

        std::cout << "Number of matched points (after cleaning): " << outMatches.size() << std::endl;

        if (refineF && outMatches.size() >= 8) {
            // The F matrix will be recomputed with all accepted matches

            // Convert keypoints into Point2f for final F computation
            points1.clear();
            points2.clear();
            for(auto const &data : outMatches){
                // Get the position of left keypoints
                points1.emplace_back(keypoints1[data.queryIdx].pt);
                // Get the position of right keypoints
                points2.emplace_back(keypoints2[data.trainIdx].pt);
            }

            // Compute 8-point F from all accepted matches
            fundemental = cv::findFundamentalMat(
                          cv::Mat(points1),cv::Mat(points2), // matching points
                          CV_FM_8POINT); // 8-point method
        }

        return fundemental;
    }

    // Match feature points using symmetry test and RANSAC
    // returns fundemental matrix
    cv::Mat match(cv::Mat& image1, cv::Mat& image2, // input images
                  std::vector<cv::DMatch>& matches, // output matches and keypoints
                  std::vector<cv::KeyPoint>& keypoints1, std::vector<cv::KeyPoint>& keypoints2) {

        // 1a. Detection of the SURF features
        featureAlgo->detect(image1, keypoints1);
        featureAlgo->detect(image2, keypoints2);

        std::cout << "Number of SURF points (1): " << keypoints1.size() << std::endl;
        std::cout << "Number of SURF points (2): " << keypoints2.size() << std::endl;

        // 1b. Extraction of the SURF descriptors
        cv::Mat descriptors1, descriptors2;
        featureAlgo->compute(image1, keypoints1, descriptors1);
        featureAlgo->compute(image2, keypoints2, descriptors2);

        std::cout << "descriptor matrix size: " << descriptors1.rows << " by " << descriptors1.cols << std::endl;

        // 2. Match the two image descriptors

        // Construction of the matcher
        cv::BFMatcher matcher(cv::NORM_L2, false);

        // from image 1 to image 2
        // based on k nearest neighbours (with k=2)
        std::vector<std::vector<cv::DMatch>> matches1;
        matcher.knnMatch(descriptors1,descriptors2,
                         matches1, // vector of matches (up to 2 per entry)
                         2);               // return 2 nearest neighbours

        // from image 2 to image 1
        // based on k nearest neighbours (with k=2)
        std::vector<std::vector<cv::DMatch>> matches2;
        matcher.knnMatch(descriptors2,descriptors1,
                         matches2, // vector of matches (up to 2 per entry)
                         2);               // return 2 nearest neighbours

        std::cout << "Number of matched points 1->2: " << matches1.size() << std::endl;
        std::cout << "Number of matched points 2->1: " << matches2.size() << std::endl;

        // 3. Remove matches for which NN ratio is > than threshold

        // clean image 1 -> image 2 matches
        int removed = ratioTest(matches1);
        std::cout << "Number of matched points 1->2 (ratio test) : " << matches1.size()<< "-removed" << std::endl;
        // clean image 2 -> image 1 matches
        removed = ratioTest(matches2);
        std::cout << "Number of matched points 1->2 (ratio test) : " << matches2.size()<< "-removed" << std::endl;

        // 4. Remove non-symmetrical matches
        std::vector<cv::DMatch> symMatches;
        symmetryTest(matches1,matches2, symMatches);

        std::cout << "Number of matched points (symmetry test): " << symMatches.size() << std::endl;

        // 5. Validate matches using RANSAC
        cv::Mat fundemental= ransacTest(symMatches, keypoints1, keypoints2, matches);

        // return the found fundemental matrix
        return fundemental;
    }
};


#endif // MATCHER_HPP
