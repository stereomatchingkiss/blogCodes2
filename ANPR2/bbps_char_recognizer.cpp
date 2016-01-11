#include "bbps_char_recognizer.hpp"
#include "utility.hpp"

#include <opencv2/imgproc.hpp>
#include <opencv2/ml.hpp>

bbps_char_recognizer::
bbps_char_recognizer(cv::Ptr<cv::ml::StatModel> rec,
                     ocv::block_binary_pixel_sum<> const &bbps) :
    bbps_(bbps),    
    rec_(rec)
{
    generate_map(bimap_);
}

std::string bbps_char_recognizer::
recognize(const cv::Mat&, const cv::Mat &binary_input)
{
    int const label =
            static_cast<int>(rec_->predict(bbps_.describe(binary_input)));
    auto it = bimap_.right.find(label);
    if(it != std::end(bimap_.right)){
        return it->second;
    }

    return {};
}
