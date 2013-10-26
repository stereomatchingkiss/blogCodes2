#include <algorithm>
#include <iterator>
#include <random>
#include <ctime>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <basicImageAlgo.hpp>

#include "detectRegions.hpp"

detectRegions::detectRegions() : saveRegions(false), showSteps(true)
{
}

std::vector<plate> detectRegions::run(cv::Mat &input)
{
    segment(input);

    return {};
}

void detectRegions::setFileName(std::string const &name)
{
    filename = name;
}

void detectRegions::setFileName(std::string &&name)
{
    filename = std::move(name);
}

/**********************************************************
 ****************** implementation ************************
 **********************************************************/

void detectRegions::cleanSegmentData()
{
    contours.clear();
    pointInterestLP.clear();
    //result.setTo(0);
}

void detectRegions::fillPlate(cv::Mat const &input, cv::RotatedRect const &rect)
{
    //For better rect cropping for each posible box
    //Make floodfill algorithm because the plate has white background
    //And then we can retrieve more clearly the contour box
    cv::circle(result, rect.center, 3, cv::Scalar(0,255,0), -1);
    //get the min size between width and height
    float minSize = (rect.size.width < rect.size.height) ? rect.size.width : rect.size.height;
    minSize = minSize - minSize * 0.5;
    //initialize rand and get 5 points around center for floodfill algorithm
    srand ( std::time(nullptr) );
    std::default_random_engine generator(std::time(nullptr));
    //Initialize floodfill parameters and variables
    floodMask.create(input.rows + 2, input.cols + 2, CV_8U);
    floodMask.setTo(0);
    int const loDiff = 30;
    int const upDiff = 30;
    int const connectivity = 4;
    int const newMaskVal = 255;
    int const NumSeeds = 10;
    int const flags = connectivity + (newMaskVal << 8 ) + CV_FLOODFILL_FIXED_RANGE + CV_FLOODFILL_MASK_ONLY;
    for(int j = 0; j != NumSeeds; ++j){
        cv::Point seed;
        std::uniform_int_distribution<int> distribution(0, static_cast<int>(minSize) - (minSize / 2));
        //seed.x = rect.center.x + distribution(generator);
        //seed.y = rect.center.y + distribution(generator);
        seed.x = rect.center.x+rand()%(int)minSize-(minSize/2);
        seed.y = rect.center.y+rand()%(int)minSize-(minSize/2);
        cv::circle(result, seed, 1, cv::Scalar(0,255,255), -1);
        cv::floodFill(input, floodMask, seed, cv::Scalar(255,0,0), nullptr, cv::Scalar::all(loDiff), cv::Scalar::all(upDiff), flags);
    }    
}

//Remove patch that are no inside limits of aspect ratio and area.
std::vector<cv::RotatedRect> detectRegions::removeContours()
{
    std::vector<cv::RotatedRect> rects;
    auto it = std::partition(std::begin(contours), std::end(contours), [&](std::vector<cv::Point> const &a)
    {
            cv::RotatedRect mr = cv::minAreaRect(a);
            if(!verifySizes(mr)){
                return false;
            }else{
                rects.emplace_back(mr);
                return true;
            }
    });
    contours.erase(it, std::end(contours));

    return rects;
}

cv::Mat detectRegions::cropImage(cv::RotatedRect const &minRect)
{
    cv::Size rect_size= minRect.size;
    if(rect_size.width < rect_size.height)
        std::swap(rect_size.width, rect_size.height);
    cv::Mat img_crop;
    cv::getRectSubPix(imgRotated, rect_size, minRect.center, img_crop);

    cv::Mat grayResult;
    grayResult.create(33, 144, CV_8UC3);
    cv::resize(img_crop, grayResult, grayResult.size(), 0, 0, cv::INTER_CUBIC);
    //Equalize croped image
    //cv::Mat grayResult;
    cv::cvtColor(grayResult, grayResult, CV_BGR2GRAY);
    cv::blur(grayResult, grayResult, cv::Size(3,3));
    cv::equalizeHist(grayResult, grayResult);

    return grayResult;
}

void detectRegions::rotatedRect(cv::Mat const &input, cv::RotatedRect const &minRect)
{
    // rotated rectangle drawing
    cv::Point2f rect_points[4];
    minRect.points(rect_points);
    for( int j = 0; j < 4; j++ ){
        cv::line( result, rect_points[j], rect_points[(j+1)%4], cv::Scalar(0, 0, 255), 1, 8 );
    }

    //Get rotation matrix
    float const aspect = static_cast<float>(minRect.size.width) / static_cast<float>(minRect.size.height);
    float angle = minRect.angle;
    if(aspect < 1){
        angle += 90;
    }
    cv::Mat const rotmat = cv::getRotationMatrix2D(minRect.center, angle, 1);

    //Create and rotate image
    cv::warpAffine(input, imgRotated, rotmat, input.size(), CV_INTER_CUBIC);    
}

std::vector<plate> detectRegions::segment(cv::Mat &input)
{
    //convert image to gray
    cv::Mat img_gray;
    cv::cvtColor(input, img_gray, CV_BGR2GRAY);
    cv::blur(img_gray, img_gray, cv::Size(5,5));

    //Finde vertical lines. Car plates have high density of vertical lines
    cv::Mat img_sobel;
    cv::Sobel(img_gray, img_sobel, CV_8U, 1, 0, 3, 1, 0, cv::BORDER_DEFAULT);
    if(showSteps){
        //cv::imshow("Sobel", img_sobel);
    }

    //threshold image
    cv::Mat img_threshold;
    cv::threshold(img_sobel, img_threshold, 0, 255, CV_THRESH_OTSU+CV_THRESH_BINARY);
    if(showSteps){
        //cv::imshow("Threshold", img_threshold);
    }

    //Morphplogic operation close
    cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(17, 3) );
    cv::morphologyEx(img_threshold, img_threshold, CV_MOP_CLOSE, element);
    if(showSteps){
        //cv::imshow("Close", img_threshold);
    }

    //Find contours of possibles plates
    //std::vector<std::vector<cv::Point> > contours;
    cv::findContours(img_threshold,
            contours, // a vector of contours
            CV_RETR_EXTERNAL, // retrieve the external contours
            CV_CHAIN_APPROX_NONE); // all pixels of each contours

    //Remove patch that are no inside limits of aspect ratio and area.
    std::vector<cv::RotatedRect> rects = removeContours();

    // Draw blue contours on a white image
    input.copyTo(result);
    cv::drawContours(result,contours,
            -1, // draw all contours
            cv::Scalar(255,0,0), // in blue
            1); // with a thickness of 1

    //cv::imshow("contours", result);

    for(auto const &rect : rects){

        //For better rect cropping for each posible box
        //Make floodfill algorithm because the plate has white background
        //And then we can retrieve more clearly the contour box
        fillPlate(input, rect);

        //Check new floodfill mask match for a correct patch.
        //Get all points detected for get Minimal rotated Rect
        for(auto begin =  floodMask.begin<uchar>(), end = floodMask.end<uchar>(); begin != end; ++begin){
            if(*begin == 255){
                pointInterestLP.emplace_back(begin.pos());
            }
        }

        cv::RotatedRect minRect = cv::minAreaRect(pointInterestLP);
        if(verifySizes(minRect)){
            //Create and rotate image
            rotatedRect(input, minRect);
            cv::imshow("rotated", imgRotated);

            //Crop image
            cv::Mat grayResult = cropImage(minRect);
            cv::imshow("result", grayResult);
            cv::waitKey();
            //grayResult=histeq(grayResult);
            //output.push_back(Plate(grayResult,minRect.boundingRect()));
        }
        pointInterestLP.clear();
    }

    if(showSteps){
        //cv::waitKey();
    }

    cleanSegmentData();

    return {};
}

bool detectRegions::verifySizes(cv::RotatedRect const &mr) const
{
    float const error = 0.4;
    //Spain car plate size: 52x11 aspect 4,7272
    float const aspect = 4.7272;
    //Set a min and max area. All other patchs are discarded
    int const min= 15 * (aspect * 15); // minimum area
    int const max= 125 * (aspect * 125); // maximum area
    //Get only patchs that match to a respect ratio.
    float const rmin = aspect - aspect * error;
    float const rmax = aspect + aspect * error;

    int const area= mr.size.height * mr.size.width;
    float r = (float)mr.size.width / (float)mr.size.height;
    if(r < 1){
        r = (float)mr.size.height / (float)mr.size.width;
    }

    if(( area < min || area > max ) || ( r < rmin || r > rmax )){
        return false;
    }else{
        return true;
    }
}
