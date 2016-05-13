#include "fixed_size_trackers.hpp"

#include <ocv_libs/saliency/utility.hpp>

#include <cmath>

fixed_size_trackers::
fixed_size_trackers(search_func search,
                    warm_func warm_strategy,
                    size_t max_player,
                    size_t miss_frame,
                    double occlusion_thresh) :
    max_player_(std::max(max_player,size_t(1))),
    miss_frame_(miss_frame),
    occlusion_thresh_(occlusion_thresh),
    search_strategy_(search),
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

size_t fixed_size_trackers::get_max_player() const
{
    return max_player_;
}

std::vector<cv::Rect2d> const&
fixed_size_trackers::get_position() const
{
    return trackers_.boundingBoxes;
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

void fixed_size_trackers::set_occlusion_thresh(double value)
{
    occlusion_thresh_ = value;
}

void fixed_size_trackers::update(const cv::Mat &input)
{
    if(!empty()){        
        auto const old_pos = trackers_.boundingBoxes;
        trackers_.update(input);
        if(should_retrack(old_pos)){
            clear();
            std::cout<<"retrack"<<std::endl;
            std::vector<cv::Rect2d> const new_roi =
                    search_strategy_(input);
            if(new_roi.size() >= max_player_){
                for(size_t i = 0; i != new_roi.size(); ++i){
                    add(input, new_roi[i], track_strategy_[i]);
                }
            }
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
                return true;
            }
        }
    }

    return false;
}
