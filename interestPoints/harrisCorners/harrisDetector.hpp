#ifndef HARRISDETECTOR_HPP
#define HARRISDETECTOR_HPP

#include <vector>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

class harrisDetector
{
public:
    harrisDetector() :
        aperture_(3),
        k_(0.01),
        max_strength_(0.0),
        neighbourhood_(3),
        non_max_size_(3),
        threshold_(0.01){}

    void detect(cv::Mat const &image)
    {
        cv::cornerHarris(image, corner_strength_, neighbourhood_, aperture_, k_);       
        cv::minMaxLoc(corner_strength_, nullptr, &max_strength_);

        cv::Mat dilated;
        cv::dilate(corner_strength_, dilated, cv::Mat());
        cv::compare(corner_strength_, dilated, local_max_, cv::CMP_EQ);
    }

    void draw_on_image(cv::Mat &image, std::vector<cv::Point> const &points,
                       cv::Scalar color = cv::Scalar(255, 255, 255),
                       int radius = 3, int thickness = 2)
    {
        for(auto const &data : points){
            cv::circle(image, data, radius, color, thickness);
        }
    }

    void get_corners(std::vector<cv::Point> &points, double quality_level)
    {
        cv::Mat corner_map = get_corner_map(quality_level);
        get_corners(points, corner_map);
    }

    void get_corners(std::vector<cv::Point> &points, cv::Mat const &corner_map)
    {
        for(int row = 0; row != corner_map.rows; ++row){
            auto row_ptr = corner_map.ptr<uchar>(row);
            for(int col = 0; col != corner_map.cols; ++col){
                if(row_ptr[col]){
                    points.emplace_back(col, row);
                }
            }
        }
    }

    cv::Mat get_corner_map(double quality_level)
    {
        threshold_ = quality_level * max_strength_;
        cv::threshold(corner_strength_, corner_th_, threshold_, 255, cv::THRESH_BINARY);
        cv::Mat corner_map;
        corner_th_.convertTo(corner_map, CV_8U);
        corner_map &= local_max_;

        return corner_map;
    }

private:
    cv::Mat corner_strength_;
    cv::Mat corner_th_;
    cv::Mat kernel_;
    cv::Mat local_max_;

    int aperture_;
    double k_;
    double max_strength_;
    int neighbourhood_;
    int non_max_size_;
    double threshold_;
};

#endif // HARRISDETECTOR_HPP
