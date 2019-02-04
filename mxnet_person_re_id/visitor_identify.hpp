#ifndef VISITOR_IDENTIFY_HPP
#define VISITOR_IDENTIFY_HPP

#include <opencv2/core.hpp>

#include <mxnet-cpp/MxNetCpp.h>

#include <memory>
#include <string>
#include <vector>

class object_detector;
class object_detector_filter;
class person_feautres_extractor;
class re_id_db;

class visitor_identify
{
public:
    struct visitor_info
    {
        float confidence_ = 0.0f;
        size_t id_ = 0;
        cv::Rect roi_;
    };

    visitor_identify(std::string const &person_re_id_params,
                     std::string const &person_re_id_symbol,
                     std::string const &obj_det_params,
                     std::string const &obj_det_symbols,
                     mxnet::cpp::Context const &context = mxnet::cpp::Context(mxnet::cpp::kGPU, 0));

    ~visitor_identify();

    std::vector<visitor_info> detect_and_identify_visitors(cv::Mat const &input);

private:
    std::unique_ptr<re_id_db> db_;
    std::unique_ptr<person_feautres_extractor> feature_extract_;
    std::unique_ptr<object_detector> obj_det_;
    std::unique_ptr<object_detector_filter> obj_filter_;
};

#endif // VISITOR_IDENTIFY_HPP
