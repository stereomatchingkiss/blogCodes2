#include "visitor_identify.hpp"

#include "../mxnet_cpp_object_detection/object_detector.hpp"
#include "../mxnet_cpp_object_detection/object_detector_filter.hpp"

#include "person_feautres_extractor.hpp"
#include "re_id_db.hpp"

visitor_identify::visitor_identify(const std::string &person_re_id_params,
                                   const std::string &person_re_id_symbol,
                                   const std::string &obj_det_params,
                                   const std::string &obj_det_symbols,
                                   const mxnet::cpp::Context &context) :
    db_(std::make_unique<re_id_db>()),
    feature_extract_(std::make_unique<person_feautres_extractor>(person_re_id_params, person_re_id_symbol, context)),
    obj_det_(std::make_unique<object_detector>(obj_det_params, obj_det_symbols, context, cv::Size(320 ,256)))
{
    std::vector<object_detector_filter::item_type> obj_to_detect;
    obj_to_detect.emplace_back(object_detector_filter::item_type::person);
    obj_filter_.reset(new object_detector_filter(std::move(obj_to_detect), cv::Size(320 ,256), 0.8f));
}

visitor_identify::~visitor_identify()
{

}

std::vector<visitor_identify::visitor_info> visitor_identify::detect_and_identify_visitors(const cv::Mat &input)
{
    obj_det_->forward(input);
    auto const detect_results = obj_filter_->filter(obj_det_->get_outputs(), cv::Size(input.cols, input.rows));

    std::vector<visitor_info> result;
    for(auto const &det : detect_results){
       auto const feature = feature_extract_->get_features(input(det.roi_).clone());
       auto const id_info = db_->find_most_similar_id(feature);
       visitor_info vinfo;
       vinfo.roi_ = det.roi_;
       if(id_info.confident_ > 0.5f){
           vinfo.id_ = id_info.id_;
           vinfo.confidence_ = id_info.confident_;
       }else{
           auto const new_id = db_->add_new_id(feature);
           vinfo.id_ = new_id;
           vinfo.confidence_ = 1.0f;
       }
       result.emplace_back(std::move(vinfo));
    }

    return result;
}
