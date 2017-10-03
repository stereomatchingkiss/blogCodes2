#include "marble_slab.hpp"

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <iostream>

/**
 * This file solve the problem at
 * https://stackoverflow.com/questions/46504836/find-contours-in-images-with-complex-background-and-rich-texture-in-opencv-3-3/46520615#46520615
 */

void marble_slab()
{
    cv::Mat input = cv::imread("../forum_quest/data/complex_bg.jpg");
    if(input.empty()){
        std::cerr<<"cannot open image\n";
        return;
    }

    cv::Mat blur_img;
    cv::GaussianBlur(input, blur_img, {11, 11}, 7);
    cv::medianBlur(blur_img, blur_img, 3);

    cv::Mat edges;
    cv::Canny(blur_img, edges, 40, 90);

    //fill up missed line of the marble slab, you can try with another
    //operation like close
    cv::Mat const kernel = cv::getStructuringElement(cv::MORPH_RECT, {7, 7});
    cv::Mat morpho;
    cv::dilate(edges, morpho, kernel);

    cv::Mat drawing = cv::Mat::zeros(morpho.size(), CV_8UC3);
    using cpoints = std::vector<cv::Point>;
    std::vector<cpoints> contours;
    cv::findContours(morpho, contours, cv::RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

    auto it = std::remove_if(std::begin(contours), std::end(contours), [](cpoints const &lhs)
    {
        return cv::boundingRect(lhs).area() < 100000;
    });
    contours.erase(it, std::end(contours));

    cv::RNG rng(12345);
    for(size_t i = 0; i != contours.size(); ++i){
        cv::Scalar const color = cv::Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
        cv::drawContours(drawing, contours, static_cast<int>(i), color, 2);
        cv::rectangle(input, cv::boundingRect(contours[i]), color, 2);
        cv::imshow("drawing", drawing);
        cv::waitKey();
    }

    cv::imshow("input", input);
    cv::imshow("blur", blur_img);
    cv::imshow("edges", edges);
    cv::imshow("morpho", morpho);
    cv::imwrite("marble_region.jpg", input);
    cv::imwrite("morho.jpg", morpho);
    cv::waitKey();
}
