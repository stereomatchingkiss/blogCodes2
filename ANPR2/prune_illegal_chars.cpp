#include "prune_illegal_chars.hpp"

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

void prune_illegal_chars::
prune(cv::Mat const &plate,
      size_t max_char_num,
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
        for(size_t i = 0; i != diff_.size(); ++i){
            contours[i] = std::move(diff_[i].second);
        }
        contours.resize(max_char_num);

        show_prune_results(plate, contours);
    }
}

void prune_illegal_chars::set_show_debug_message(bool value)
{
    debug_ = value;
}

void prune_illegal_chars::show_prune_results(cv::Mat const &plate,
                                             contours_type const &contours) const
{
    if(debug_){
        cv::Mat zero = cv::Mat::zeros(plate.size(),CV_8U);
        for(int i = 0; i != contours.size(); ++i){
            cv::drawContours(zero, contours, i, {255}, -1);
        }
        cv::imshow("prune_results", zero);
        cv::waitKey();
        cv::destroyWindow("prune_results");
    }
}
