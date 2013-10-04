#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/nonfree/features2d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

//not a big deal to use global parameter in this small program
std::string const Folder("/Users/Qt/program/blogsCodes/pic/");

void sift_detect(cv::Mat const &image)
{
    std::vector<cv::KeyPoint> keypoints;
    //cv::SurfFeatureDetector surf(2500);
    cv::SIFT sift;
    sift.detect(image, keypoints);
    cv::Mat result;
    cv::drawKeypoints(image, keypoints, result, cv::Scalar(255), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

    cv::imshow("sift", result);

    cv::waitKey();
}

void surf_detect(cv::Mat const &image)
{
    std::vector<cv::KeyPoint> keypoints;
    //cv::SurfFeatureDetector surf(2500);
    cv::SURF surf(2500);
    surf.detect(image, keypoints);
    cv::Mat result;
    cv::drawKeypoints(image, keypoints, result, cv::Scalar(255), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

    cv::imshow("surf", result);

    cv::waitKey();
}

template<typename DetectAndDescript>
void detect_and_match(cv::Mat const &image1, cv::Mat const &image2, DetectAndDescript &&detect)
{    
    //cv::SURF surf(2500);
    std::vector<cv::KeyPoint> keypoints1;
    cv::Mat descriptors1;
    detect(image1, cv::Mat(), keypoints1, descriptors1);

    std::vector<cv::KeyPoint> keypoints2;
    cv::Mat descriptors2;
    detect(image2, cv::Mat(), keypoints2, descriptors2);
    //std::cout<<descriptors1.rows<<", "<<descriptors1.cols<<std::endl;
    //std::cout<<descriptors2.rows<<", "<<descriptors2.cols<<std::endl;

    //-- Step 3: Matching descriptor vectors with a brute force matcher
    cv::BFMatcher matcher(cv::NORM_L2, false);
    std::vector<cv::DMatch> matches;
    matcher.match(descriptors1, descriptors2, matches);

    /*if(matches.size() > 25){
        std::nth_element(std::begin(matches), std::begin(matches) + 25, std::end(matches));
        matches.erase(std::begin(matches) + 25, std::end(matches));
    }*/

    //-- Draw matches
    cv::Mat img_matches;
    cv::drawMatches(image1, keypoints1, image2, keypoints2, matches, img_matches, cv::Scalar(255));

    //-- Show detected matches
    imshow("Matches", img_matches);

    cv::waitKey(0);
}

int main()
{
    std::string const name1[] = {"leafResult04.png", "church01.jpg"};
    std::string const name2[] = {"leafResult00.png", "church02.jpg"};
    size_t const imageIndex = 0;

    cv::Mat image1 = cv::imread(Folder + name1[imageIndex], CV_LOAD_IMAGE_GRAYSCALE);
    if(image1.empty()){
        std::cerr<<"input 1 is empty"<<std::endl;
        return -1;
    }

    //sift_detect(name1[imageIndex]);
    //surf_detect(name1[imageIndex]);

    cv::Mat image2 = cv::imread(Folder + name2[imageIndex], CV_LOAD_IMAGE_GRAYSCALE);
    if(image2.empty()){
        std::cerr<<"input 2 is empty"<<std::endl;
        return -1;
    }

    detect_and_match(image1, image2, cv::SURF(2500));
    detect_and_match(image1, image2, cv::SIFT());

    return 0;
}

