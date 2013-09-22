#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/tracking.hpp>

#include <imageAlgorithm.hpp> //for OCV::histProject()

int main()
{
    cv::Mat input = cv::imread("/Users/Qt/program/blogsCodes/pic/monkey00.png");
    cv::Mat target = cv::imread("/Users/Qt/program/blogsCodes/pic/monkey02.png");
    if(!input.empty() && !target.empty()){
        cv::Rect const roi_initial_location(240, 56, 86, 72); //initial search window(same as the region of interest)
        cv::Mat roi = input(roi_initial_location).clone();

        //find out the back projection histogram base on roi and target(the probability of roi occur on target)
        cv::Mat probability_map = OCV::histProject().get_projection_map_hue(target, roi);
        cv::TermCriteria criteria(cv::TermCriteria::COUNT + cv::TermCriteria::EPS, 10, 0.01);

        //use mean shift to find the roi
        cv::Rect mean_shift_window = roi_initial_location;
        cv::meanShift(probability_map, mean_shift_window, criteria);

        //use cam shift to find the roi
        cv::Rect cam_shift_window = roi_initial_location;
        cv::CamShift(probability_map, cam_shift_window, criteria);

        //draw the region of interest on input
        cv::rectangle(input, roi_initial_location, cv::Scalar(0, 0, 255));
        //draw the initial window location(red) and the location obtained by meanshift(green) and camshift(blue)
        cv::rectangle(target, roi_initial_location, cv::Scalar(0, 0, 255));
        cv::rectangle(target, mean_shift_window, cv::Scalar(0, 255, 0));
        cv::rectangle(target, cam_shift_window, cv::Scalar(255, 0, 0));

        cv::imshow("roi", input);
        cv::imshow("target", target);
        cv::imwrite("roi.png", input);
        cv::imwrite("target.png", target);
        cv::waitKey();
    }else{
        std::cerr<<"input or target is empty"<<std::endl;
    }

    return 0;
}
