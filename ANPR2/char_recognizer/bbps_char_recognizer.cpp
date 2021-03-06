#include "bbps_char_recognizer.hpp"
#include "../utility/utility.hpp"

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/ml.hpp>

bbps_char_recognizer::
bbps_char_recognizer(cv::Ptr<cv::ml::StatModel> alpha_rec,
                     cv::Ptr<cv::ml::StatModel> num_rec,
                     ocv::block_binary_pixel_sum<> const &bbps) :
    alpha_rec_(alpha_rec),
    bbps_(bbps),
    num_rec_(num_rec)
{
    generate_map(alpha_bimap_, map_type::alpahbet);
    generate_map(num_bimap_, map_type::number);
}

std::string bbps_char_recognizer::
recognize(cv::Mat const&, cv::Mat const &binary_input,
          regions_type const &regions)
{
    std::string result;
    if(regions.size() >= 6){
        for(size_t i = 0; i != 2; ++i){
            result += recognize(binary_input(regions[i]),
                                alpha_rec_, alpha_bimap_);
        }
        for(size_t i = 2; i != 5; ++i){
            result += recognize(binary_input(regions[i]),
                                num_rec_, num_bimap_);
        }
        result += recognize(binary_input(regions[5]),
                            alpha_rec_, alpha_bimap_);
    }

    return result;
}

std::string bbps_char_recognizer::
recognize(cv::Mat const& character,
          cv::Ptr<cv::ml::StatModel> const &rec,
          boost::bimap<std::string, int> const &map)
{
    int const label =
            static_cast<int>(rec->predict(bbps_.describe(character)));
    auto it = map.right.find(label);
    if(it != std::end(map.right)){
        return it->second;
    }

    return {};
}
