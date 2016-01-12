#ifndef ANPR_RECOGNIZER_HPP
#define ANPR_RECOGNIZER_HPP

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

#include <algorithm>
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
    anpr_recognizer(PlateLocalizer const &plate_localizer,
                    SegmentChar const &segment_char,
                    PruneChar const &prune_char,
                    CharRecognizer const &char_recognizer,
                    PatternRecognizer const &pattern_recognizer);

    std::vector<std::string> const& recognize(cv::Mat const &input);


private:
    PlateLocalizer plate_localizer_;
    SegmentChar segment_char_;
    PruneChar prune_char_;
    CharRecognizer char_recognizer_;
    PatternRecognizer pattern_recognizer_;

    std::vector<std::string> plate_chars_;
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
std::vector<std::string> const& anpr_recognizer<PlateLocalizer, SegmentChar,
PruneChar, CharRecognizer, PatternRecognizer>::
recognize(cv::Mat const &input)
{
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
                for(size_t i = 0; i != prune_contours.size(); ++i){
                    auto const &rect = cv::boundingRect(prune_contours[i]);
                    plate_chars +=
                            char_recognizer_.recognize(bird_eyes_plate(rect),
                                                       binary_plate(rect));
                }
                auto strs = pattern_recognizer_.fit(plate_chars);
                if(!strs.empty()){
                    plate_chars_.emplace_back(std::move(strs));
                }//*/
            }
        }
    }//*/

    return plate_chars_;
}


#endif
