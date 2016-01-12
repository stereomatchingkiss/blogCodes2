#ifndef ANPR_RECOGNIZER_HPP
#define ANPR_RECOGNIZER_HPP

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

template<typename PlateLocalizer,
         typename SegmentChar,
         typename PruneChar,
         typename CharRecognizer,
         typename PatternRecognizer>
class anpr_recognizer
{
public:
    using result_type = std::vector<std::pair<std::string, cv::Rect>>;

    anpr_recognizer(PlateLocalizer const &plate_localizer,
                    SegmentChar const &segment_char,
                    PruneChar const &prune_char,
                    CharRecognizer const &char_recognizer,
                    PatternRecognizer const &pattern_recognizer);

    std::vector<cv::Mat> const& get_binary_plate() const
    {
        return binary_plates_;
    }

    result_type const& recognize(cv::Mat const &input);


private:
    std::vector<cv::Mat> binary_plates_;

    PlateLocalizer plate_localizer_;
    SegmentChar segment_char_;
    PruneChar prune_char_;
    CharRecognizer char_recognizer_;
    PatternRecognizer pattern_recognizer_;

    result_type plate_chars_;
};

template<typename PlateLocalizer,
         typename SegmentChar,
         typename PruneChar,
         typename CharRecognizer,
         typename PatternRecognizer>
anpr_recognizer<PlateLocalizer, SegmentChar,
PruneChar, CharRecognizer, PatternRecognizer>::
anpr_recognizer(PlateLocalizer const &plate_localizer,
                SegmentChar const &segment_char,
                PruneChar const &prune_char,
                CharRecognizer const &char_recognizer,
                PatternRecognizer const &pattern_recognizer) :
    plate_localizer_(plate_localizer),
    segment_char_(segment_char),
    prune_char_(prune_char),
    char_recognizer_(char_recognizer),
    pattern_recognizer_(pattern_recognizer)
{

}

template<typename PlateLocalizer,
         typename SegmentChar,
         typename PruneChar,
         typename CharRecognizer,
         typename PatternRecognizer>
std::vector<std::pair<std::string, cv::Rect>> const& anpr_recognizer<PlateLocalizer, SegmentChar,
PruneChar, CharRecognizer, PatternRecognizer>::
recognize(cv::Mat const &input)
{
    binary_plates_.clear();
    plate_chars_.clear();
    plate_localizer_.localize_plate(input);
    for(auto const &contour : plate_localizer_.get_contours()){
        if(segment_char_.detect_characters(plate_localizer_.get_resize_input(),
                                           contour)){
            auto const &bird_eyes_plate = segment_char_.get_bird_eyes_plate();
            prune_char_.prune(segment_char_.get_bird_eyes_plate(),
                              segment_char_.get_chars_contours());
            auto &prune_contours = segment_char_.get_chars_contours();
            if(prune_contours.size() >= segment_char_.get_min_char_num()){
                pattern_recognizer_.sort_contour(prune_contours);
                std::string plate_chars;
                auto const &binary_plate = segment_char_.get_binary_plate();
                binary_plates_.emplace_back(binary_plate);
                for(size_t i = 0; i != prune_contours.size(); ++i){
                    auto const &rect = cv::boundingRect(prune_contours[i]);
                    plate_chars +=
                            char_recognizer_.recognize(bird_eyes_plate(rect),
                                                       binary_plate(rect));
                }                
                auto strs = pattern_recognizer_.fit(plate_chars);
                if(!strs.empty()){
                    std::cout<<__func__<<" : can detect plate"<<std::endl;
                    plate_chars_.emplace_back(std::move(strs),
                                              cv::boundingRect(contour));
                }else{
                    std::cout<<__func__<<" : cannot detect plate"<<std::endl;
                    plate_chars_.emplace_back(std::move(plate_chars),
                                              cv::boundingRect(contour));
                }
            }
        }
    }

    return plate_chars_;
}


#endif
