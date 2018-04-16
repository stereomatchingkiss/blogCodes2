#include "feature_extractor_resnet34.hpp"

#include "resnet34.hpp"

#include <dlib/data_io.h>

using namespace dlib;

struct feature_extractor_resnet34::pimpl
{
    resnet34::anet_type net_;
};

feature_extractor_resnet34::feature_extractor_resnet34(std::string const &file_location) :
    pimpl_(new feature_extractor_resnet34::pimpl)
{    
    dlib::deserialize(file_location)>>pimpl_->net_;
}

feature_extractor_resnet34::~feature_extractor_resnet34()
{

}

dlib::matrix<float>
feature_extractor_resnet34::get_feature(const img_type &input)
{
    pimpl_->net_(input);

    return dlib::mat(dlib::layer<2>(pimpl_->net_).get_output());
}

std::vector<dlib::matrix<float>>
feature_extractor_resnet34::get_features(const std::vector<img_type> &input)
{
    std::vector<dlib::matrix<float>> results;
    for(auto const &img : input){
        results.emplace_back(get_feature(img));
    }

    return results;
}
