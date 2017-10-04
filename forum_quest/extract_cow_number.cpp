#include "extract_cow_number.hpp"

#include <ocv_libs/core/attribute.hpp>

#include <opencv2/core.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/objdetect.hpp>

#include <iostream>

/**
 *solve problem post at https://stackoverflow.com/questions/29923827/extract-cow-number-from-image
 */

using namespace std;
using namespace cv;

using cpoints = vector<Point>;

namespace{

cv::Mat fore_ground_extract(cv::Mat const &input)
{
    vector<Mat> bgr;
    split(input, bgr);
    Mat text_region = bgr[0];
    blur(text_region, text_region, {3, 3});
    threshold(text_region, text_region, 0, 255, cv::THRESH_OTSU);
    medianBlur(text_region, text_region, 5);
    Mat const erode_kernel = getStructuringElement(MORPH_ELLIPSE, {11, 11});
    erode(text_region, text_region, erode_kernel);
    Mat const dilate_kernel = getStructuringElement(MORPH_ELLIPSE, {7, 7});
    dilate(text_region, text_region, dilate_kernel);
    bitwise_not(text_region, text_region);

    return text_region;
}

std::vector<std::vector<cv::Point>> get_text_contours(cv::Mat const &input)
{
    vector<cpoints> contours;

    findContours(input, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    auto it = std::remove_if(std::begin(contours), std::end(contours), [](cpoints const &cp)
    {
        auto const area = cv::boundingRect(cp).area();

        return area < 900 || area >= 10000;
    });
    contours.erase(it, std::end(contours));

    return contours;
}

}

void extract_cow_number()
{
    Mat input = imread("../forum_quest/data/cow_02.jpg");
    cout<<input.size()<<endl;
    if(input.empty()){
        cerr<<"cannot open image\n";
        return;
    }
    if(input.cols >= 1500){
        cv::resize(input, input, {}, 0.25, 0.25);
    }

    Mat crop_region;
    input(Rect(0, 0, input.cols, std::min(input.rows/3, input.rows))).copyTo(crop_region);

    Mat text_region = fore_ground_extract(crop_region);
    vector<cpoints> const text_contours = get_text_contours(text_region);
    RNG rng(12345);
    Mat text_mask(text_region.size(), CV_8UC3);
    //ocv::print_contour_attribute_name(cout);
    for(size_t i = 0; i < text_contours.size(); ++i){
        Scalar const color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
        drawContours(text_mask, text_contours, static_cast<int>(i), color, 2);
        rectangle(crop_region, boundingRect(text_contours[i]), color, 2);
        //drawContours(text_region, text_contours, static_cast<int>(i), color, 2);
        //ocv::print_contour_attribute(text_contours[i], 0.001, cout);
        //imshow("text_mask", text_mask);
        //waitKey();
    }

    imshow("crop", crop_region);
    imshow("text_region", text_region);
    imshow("text mask", text_mask);
    waitKey();
}
