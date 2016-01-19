#include "fhog_localizer.hpp"

#include <ocv_libs/core/resize.hpp>

#include <dlib/opencv/cv_image.h>
#include <dlib/gui_widgets.h>

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

fhog_localizer::fhog_localizer()
{
    detector_ = std::make_shared<plate_detector_type>();
    dlib::deserialize("number_plate_detector.svm")>>*detector_;
}

const fhog_localizer::contours_type &fhog_localizer::
get_contours() const
{
    return contours_;
}

cv::Mat& fhog_localizer::get_resize_input()
{
    return resize_mat_;
}

cv::Mat const& fhog_localizer::get_resize_input() const
{
    return resize_mat_;
}

void fhog_localizer::localize_plate(const cv::Mat &input)
{
    ocv::resize_aspect_ratio(input, resize_mat_, {640,0});
    cv::cvtColor(resize_mat_, gray_mat_, CV_BGR2GRAY);
    contours_.clear();

    using namespace dlib;

    // Run the detector and get the number plate detections
    dlib::cv_image<unsigned char> img(gray_mat_);
    std::vector<rectangle> dets = (*detector_)(img);
    for(rectangle &rect : dets){
        //increase the size of possible plate region, this could
        //help us locate full plate informations
        rectangle const tg(rect.left() - rect.left() * 0.15,
                           rect.top() - rect.top() * 0.05,
                           rect.right() + rect.right() * 0.15,
                           rect.bottom() + rect.bottom() * 0.05);
        rect = tg;
        contour_type contour{{rect.left(), rect.top()},
                             {rect.right(), rect.top()},
                             {rect.left(), rect.bottom()},
                             {rect.right(), rect.bottom()}};
        contours_.emplace_back(std::move(contour));
    }

    if(debug_){
        /*image_window win;
        win.clear_overlay();
        dlib::cv_image<dlib::bgr_pixel> color_img(resize_mat_);
        win.set_image(color_img);
        win.add_overlay(dets, rgb_pixel(255,0,0));
        win.wait_until_closed();//*/

        for(auto const &contour : contours_){
            cv::imshow("localize", resize_mat_(cv::boundingRect(contour)));
            cv::waitKey();
            cv::destroyAllWindows();
        }
    }
}

void fhog_localizer::set_show_debug_message(bool value)
{
    debug_ = value;
}
