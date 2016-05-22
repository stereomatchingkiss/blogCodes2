#include "fixed_size_trackers.hpp"

#include <ocv_libs/saliency/utility.hpp>

#include <cmath>

fixed_size_trackers::
fixed_size_trackers(search_func search,
                    warm_func warm_strategy,
                    size_t track_size,
                    size_t miss_frame,
                    size_t occlusion_frame,
                    double occlusion_thresh) :    
    miss_frame_(miss_frame),
    occlusion_thresh_(occlusion_thresh),
    occlusion_frame_(occlusion_frame),
    occlusion_record_(0),
    search_strategy_(search),
    target_was_lost_(false),
    track_size_(std::max(track_size,size_t(1))),
    warm_strategy_(warm_strategy)
{

}

void fixed_size_trackers::
add(cv::Mat const &input,
    cv::Rect2d const &roi,
    std::string const &strategy)
{
    miss_records_.emplace_back(0);
    trackers_.addTarget(input, roi, strategy);
    track_strategy_.emplace_back(strategy);
}

void fixed_size_trackers::clear()
{
    miss_records_.clear();
    trackers_.trackers.clear();
    trackers_.boundingBoxes.clear();
    trackers_.colors.clear();
    occlusion_record_ = 0;
}

bool fixed_size_trackers::empty() const
{
    return trackers_.trackers.empty();
}

size_t fixed_size_trackers::get_miss_frame() const
{
    return miss_frame_;
}

double fixed_size_trackers::get_occlusion_thresh() const
{
    return occlusion_thresh_;
}

size_t fixed_size_trackers::get_track_size() const
{
    return track_size_;
}

size_t fixed_size_trackers::get_occlusion_frame() const
{
    return occlusion_frame_;
}

std::vector<cv::Rect2d> const&
fixed_size_trackers::get_position() const
{
    return trackers_.boundingBoxes;
}

void fixed_size_trackers::set_max_player(size_t max_player)
{
    track_size_ = std::max(size_t(1), max_player);
}

cv::Rect2d fixed_size_trackers::get_position(size_t target) const
{
    if(target < trackers_.boundingBoxes.size()){
        return trackers_.boundingBoxes[target];
    }

    return {};
}

void fixed_size_trackers::set_miss_frame(size_t value)
{
    miss_frame_ = value;
}

void fixed_size_trackers::set_occlusion_frame(size_t value)
{
    occlusion_frame_ = value;
}

void fixed_size_trackers::set_occlusion_thresh(double value)
{
    occlusion_thresh_ = value;
}

void fixed_size_trackers::retrack(const cv::Mat &input)
{
    clear();
    static size_t rt = 0;
    std::cout<<"retrack : "<<rt++<<std::endl;
    std::vector<cv::Rect2d> const new_roi =
            search_strategy_(input);
    if(new_roi.size() >= track_size_){
        for(size_t i = 0; i != new_roi.size(); ++i){
            add(input, new_roi[i], track_strategy_[i]);
        }
        occlusion_record_ = 0;
    }
}

void fixed_size_trackers::update(const cv::Mat &input)
{
    if(!empty() || target_was_lost_){
        auto const old_pos = trackers_.boundingBoxes;
        trackers_.update(input);
        if(should_retrack(old_pos)){
            target_was_lost_ = true;
            retrack(input);
        }else{
            warm_strategy_(input);
        }
    }       
}

bool fixed_size_trackers::
should_retrack(const std::vector<cv::Rect2d> &old_pos)
{
    if(miss_frame_ > 0){
        for(size_t i = 0; i != old_pos.size(); ++i){
            if(old_pos[i] == trackers_.boundingBoxes[i]){
                ++miss_records_[i];
                if(miss_records_[i] >= miss_frame_){
                    return true;
                }
            }else{
                miss_records_[i] = 0;
            }
        }
    }

    if(trackers_.boundingBoxes.size() > 1){
        auto const &box = trackers_.boundingBoxes;
        for(size_t i = 0; i != box.size()-1; ++i){
            auto const func = [&](cv::Rect2d const &val)
            {
                return ocv::saliency::calculate_iou(val, box[i]) >
                        occlusion_thresh_;
            };
            auto const occlusion =
                    std::any_of(std::begin(box) + i + 1,
                                std::end(box), func);
            if(occlusion){
                ++occlusion_record_;
            }else{
                occlusion_record_ = 0;
            }

            return occlusion && (occlusion_record_ >= occlusion_frame_);
        }
    }

    if(empty() && target_was_lost_){
        return true;
    }

    return false;
}
