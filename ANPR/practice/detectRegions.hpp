#ifndef DETECTREGIONS_HPP
#define DETECTREGIONS_HPP

#include <string>
#include <vector>

#include "plate.hpp"

class detectRegions
{
public:
    detectRegions();

    std::vector<plate>& run(cv::Mat &input);   

private:
    void cleanSegmentData();
    cv::Mat cropImage(cv::RotatedRect const &minRect);

    void fillPlate(cv::Mat const &input, cv::RotatedRect const &rect);

    std::vector<cv::RotatedRect> removeContours();
    void rotatedRect(cv::Mat const &input, cv::RotatedRect const &minRect);

    std::vector<plate>& segment(cv::Mat &input);

    bool verifySizes(cv::RotatedRect const &mr) const;

private:
    std::vector<std::vector<cv::Point>> contours;

    cv::Mat floodMask;
    cv::Mat imgRotated;
    std::vector<plate> plates;
    std::vector<cv::Point> pointInterestLP; //interest point of license plate after floodfill
    cv::Mat result;
    bool showSteps;
};

#endif // DETECTREGIONS_HPP
