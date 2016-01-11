#include "prune_illegal_chars.hpp"

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <iostream>

prune_illegal_chars::prune_illegal_chars(size_t max_char_num) :
    max_char_num_(max_char_num){}

void prune_illegal_chars::
prune(const cv::Mat &plate, contours_type &contours)
{
    prune(plate, max_char_num_, contours);
}

void prune_illegal_chars::
prune(cv::Mat const &plate,
      size_t,
      contours_type &contours)
{        
    auto func = [](auto const &v){ return cv::boundingRect(v).width < 6; };
    auto rit = std::remove_if(std::begin(contours), std::end(contours),
                              func);
    contours.erase(rit, std::end(contours));

    //prune_fix_num_plate(max_char_num, contours);
    show_prune_results(plate, contours);
}

void prune_illegal_chars::set_show_debug_message(bool value)
{
    debug_ = value;
}

void prune_illegal_chars::
prune_fix_num_plate(size_t max_char_num,
                    contours_type &contours)
{
    if(contours.size() > max_char_num){
        dims_.resize(0);
        for(int i = 0; i != contours.size(); ++i){
            auto const rect =cv::boundingRect(contours[i]);
            dims_.push_back(rect.height + rect.y);
        }

        diff_.clear();
        for(int r = 0; r != dims_.rows; ++r){
            cv::absdiff(dims_, dims_.at<int>(r,0), dims_);
            diff_.emplace_back(cv::sum(dims_)[0], std::move(contours[r]));
        }
        std::sort(std::begin(diff_), std::end(diff_),
                  [](auto const &lhs, auto const &rhs)
        {
            return lhs.first< rhs.first;
        });
        for(size_t i = 0; i != max_char_num; ++i){
            contours[i] = std::move(diff_[i].second);
        }
    }else{
        if(debug_){
            std::cout<<"nothing to prune"<<std::endl;
        }
    }
}

void prune_illegal_chars::show_prune_results(cv::Mat const &plate,
                                             contours_type const &contours) const
{
    if(debug_){
        for(int i = 0; i != contours.size(); ++i){
            auto const rect =cv::boundingRect(contours[i]);
            std::cout<<rect<<std::endl;
            cv::Mat plate_copy = plate.clone();
            cv::drawContours(plate_copy, contours, i, {0,255,0},2);
            cv::imshow("plate", plate_copy);
            cv::imshow("chars", plate(rect));
            cv::waitKey();
            cv::destroyAllWindows();
        }

        cv::Mat zero = cv::Mat::zeros(plate.size(),CV_8U);
        for(int i = 0; i != contours.size(); ++i){
            cv::drawContours(zero, contours, i, {255}, -1);
        }
        cv::imshow("plate", plate);
        cv::imshow("prune_results", zero);
        cv::waitKey();
        cv::destroyWindow("prune_results");
        cv::destroyWindow("plate");
    }
}
