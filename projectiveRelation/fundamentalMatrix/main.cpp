#include <iostream>
#include <string>
#include <vector>

#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/nonfree/nonfree.hpp>

#include "matcher.hpp"

std::string const Folder("/Users/Qt/program/blogsCodes/pic/");

void basic()
{
    // Read input images
    cv::Mat image1= cv::imread(Folder + "church01.jpg", CV_LOAD_IMAGE_GRAYSCALE);
    cv::Mat image2= cv::imread(Folder + "church03.jpg", CV_LOAD_IMAGE_GRAYSCALE);
    if (!image1.data || !image2.data)
        return;

    // Display the images
    cv::imshow("Right Image",image1);
    cv::imshow("Left Image",image2);

    // vector of keypoints
    std::vector<cv::KeyPoint> keypoints1;
    std::vector<cv::KeyPoint> keypoints2;

    cv::SURF surf(3000);
    surf.detect(image1, keypoints1);
    surf.detect(image2, keypoints2);

    std::cout << "Number of SURF points (1): " << keypoints1.size() << std::endl;
    std::cout << "Number of SURF points (2): " << keypoints2.size() << std::endl;

    // Draw the kepoints
    cv::Mat imageKP;
    cv::drawKeypoints(image1, keypoints1, imageKP, cv::Scalar(255,255,255), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
    //cv::imshow("Right SURF Features",imageKP);
    cv::drawKeypoints(image2, keypoints2, imageKP, cv::Scalar(255,255,255), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
    //cv::imshow("Left SURF Features",imageKP);

    // Extraction of the SURF descriptors
    cv::Mat descriptors1, descriptors2;
    surf.compute(image1, keypoints1, descriptors1);
    surf.compute(image2, keypoints2, descriptors2);

    std::cout << "descriptor matrix size: " << descriptors1.rows << " by " << descriptors1.cols << std::endl;

    cv::BFMatcher matcher(cv::NORM_L2, false);
    std::vector<cv::DMatch> matches;
    matcher.match(descriptors1, descriptors2, matches);

    //-- Draw matches
    //cv::Mat img_matches;
    //cv::drawMatches(image1, keypoints1, image2, keypoints2, matches, img_matches, cv::Scalar(255));
    //cv::imshow("img matches", img_matches);

    // Select few Matches
    std::cout<<"matches size = "<<matches.size()<<std::endl;
    std::vector<cv::DMatch> selMatches{matches[14], matches[16], matches[141],
                matches[146], matches[235], matches[238], matches[100],
                matches[262]};

    // Draw the selected matches
    cv::Mat imageMatches;
    cv::drawMatches(image1,keypoints1,  // 1st image and its keypoints
                    image2,keypoints2,  // 2nd image and its keypoints
                    //selMatches,                     // the matches
                    matches,                        // the matches
                    imageMatches,           // the image produced
                    cv::Scalar(255,255,255)); // color of the lines
    cv::imshow("Matches",imageMatches);

    // Convert 1 vector of keypoints into
    // 2 vectors of Point2f
    std::vector<int> pointIndexes1;
    std::vector<int> pointIndexes2;
    for (auto const &data : selMatches) {

        // Get the indexes of the selected matched keypoints
        pointIndexes1.push_back(data.queryIdx);
        pointIndexes2.push_back(data.trainIdx);
    }

    // Convert keypoints into Point2f
    std::vector<cv::Point2f> selPoints1, selPoints2;
    cv::KeyPoint::convert(keypoints1, selPoints1, pointIndexes1);
    cv::KeyPoint::convert(keypoints2, selPoints2, pointIndexes2);

    // check by drawing the points
    for (auto const &data : selPoints1) {

        // draw a circle at each corner location
        cv::circle(image1, data, 3, cv::Scalar(255, 255, 255), 2);
    }

    for (auto const &data : selPoints2) {

        // draw a circle at each corner location
        cv::circle(image2, data, 3, cv::Scalar(255, 255, 255), 2);
    }

    // Compute F matrix from 7 matches
    cv::Mat fundemental= cv::findFundamentalMat(
                cv::Mat(selPoints1), // points in first image
                cv::Mat(selPoints2), // points in second image
                CV_FM_8POINT);       // 7-point method

    std::cout << "F-Matrix size= " << fundemental.rows << "," << fundemental.cols << std::endl;

    // draw the left points corresponding epipolar lines in right image
    std::vector<cv::Vec3f> lines1;
    cv::computeCorrespondEpilines(
                cv::Mat(selPoints1), // image points
                1,                   // in image 1 (can also be 2)
                fundemental, // F matrix
                lines1);     // vector of epipolar lines

    // for all epipolar lines
    for (auto const &data : lines1) {

        // draw the epipolar line between first and last column
        cv::line(image2, cv::Point(0, -data[2] / data[1]),
                cv::Point(image2.cols, -(data[2] + data[0]* image2.cols) / data[1]),
                cv::Scalar(255, 255, 255) );
    }

    // draw the left points corresponding epipolar lines in left image
    std::vector<cv::Vec3f> lines2;
    cv::computeCorrespondEpilines(cv::Mat(selPoints2),2,fundemental,lines2);
    for (auto const &data : lines2) {

        // draw the epipolar line between first and last column
        cv::line(image1,cv::Point(0, -data[2] / data[1]),
                cv::Point(image1.cols,-(data[2] + data[0] * image1.cols) / data[1]),
                cv::Scalar(255, 255, 255) );
    }

    // Display the images with points and epipolar lines
    cv::imshow("Right Image Epilines",image1);
    cv::imshow("Left Image Epilines",image2);

    cv::waitKey();
}

void ransac()
{
    // Read input images
    cv::Mat image1= cv::imread(Folder + "canal1.jpg", CV_LOAD_IMAGE_GRAYSCALE);
    cv::Mat image2= cv::imread(Folder + "canal2.jpg", CV_LOAD_IMAGE_GRAYSCALE);
    if (!image1.data || !image2.data)
        return 0;

    // Display the images
    cv::imshow("Right Image",image1);
    cv::imshow("Left Image",image2);

    // Prepare the matcher
    RobustMatcher rmatcher;
    rmatcher.setConfidenceLevel(0.98);
    rmatcher.setMinDistanceToEpipolar(1.0);
    rmatcher.setRatio(0.65f);

    // Match the two images
    std::vector<cv::DMatch> matches;
    std::vector<cv::KeyPoint> keypoints1, keypoints2;
    cv::Mat fundemental = rmatcher.match(image1,image2, matches, keypoints1, keypoints2);

    // draw the matches
    cv::Mat imageMatches;
    cv::drawMatches(image1,keypoints1,  // 1st image and its keypoints
                    image2,keypoints2,  // 2nd image and its keypoints
                    matches,                        // the matches
                    imageMatches,           // the image produced
                    cv::Scalar(255,255,255)); // color of the lines
    cv::namedWindow("Matches");
    cv::imshow("Matches",imageMatches);

    // Convert keypoints into Point2f
    std::vector<cv::Point2f> points1, points2;
    for (auto it= matches.begin(); it!= matches.end(); ++it) {

        // Get the position of left keypoints
        float x= keypoints1[it->queryIdx].pt.x;
        float y= keypoints1[it->queryIdx].pt.y;
        points1.push_back(keypoints1[it->queryIdx].pt);
        cv::circle(image1, cv::Point(x,y), 3, cv::Scalar(255,255,255), 3);
        // Get the position of right keypoints
        x= keypoints2[it->trainIdx].pt.x;
        y= keypoints2[it->trainIdx].pt.y;
        cv::circle(image2, cv::Point(x,y), 3, cv::Scalar(255,255,255), 3);
        points2.push_back(cv::Point2f(x,y));
    }

    // Draw the epipolar lines
    std::vector<cv::Vec3f> lines1;
    cv::computeCorrespondEpilines(cv::Mat(points1), 1, fundemental,lines1);

    for (auto it= lines1.begin();
         it!=lines1.end(); ++it) {

        cv::line(image2,cv::Point(0,-(*it)[2]/(*it)[1]),
                cv::Point(image2.cols,-((*it)[2]+(*it)[0]*image2.cols)/(*it)[1]),
                cv::Scalar(255,255,255));
    }

    std::vector<cv::Vec3f> lines2;
    cv::computeCorrespondEpilines(cv::Mat(points2),2,fundemental,lines2);

    for (auto it= lines2.begin();
         it!=lines2.end(); ++it) {

        cv::line(image1,cv::Point(0,-(*it)[2]/(*it)[1]),
                cv::Point(image1.cols,-((*it)[2]+(*it)[0]*image1.cols)/(*it)[1]),
                cv::Scalar(255,255,255));
    }

    // Display the images with epipolar lines
    cv::namedWindow("Right Image Epilines (RANSAC)");
    cv::imshow("Right Image Epilines (RANSAC)",image1);
    cv::namedWindow("Left Image Epilines (RANSAC)");
    cv::imshow("Left Image Epilines (RANSAC)",image2);


    cv::waitKey();
}

int main()
{
    ransac();

    return 0;
}
