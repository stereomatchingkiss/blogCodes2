#include "re_id_db.hpp"

#include "compare_features_distance.hpp"

using namespace mxnet_aux;

re_id_db::re_id_db() :
    compare_(std::make_unique<cosine_similarity>())
{    
}

re_id_db::~re_id_db()
{

}

size_t re_id_db::add_new_id(cv::Mat_<float> const &input)
{
    features_.emplace_back(input);
    return features_.size() - 1;
}

re_id_db::id_info re_id_db::find_most_similar_id(cv::Mat_<float> const &input)
{
    id_info result;    
    for(size_t i = 0; i != features_.size(); ++i){
        auto const confident = compare_->compare_feature(features_[i], input);
        if(confident > result.confident_){
            result.confident_ = confident;
            result.id_ = i;
        }
    }

    return result;
}
