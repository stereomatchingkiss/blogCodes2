#ifndef PERSON_FEAUTRES_EXTRACTOR_HPP
#define PERSON_FEAUTRES_EXTRACTOR_HPP

#include <opencv2/core.hpp>

#include <memory>
#include <string>

namespace mxnet{

namespace cpp{

class Context;
class Executor;

}

}

namespace ocv{

namespace img_cvt{

class opencv_to_ndarray;

}

}

class person_feautres_extractor
{
public:
    person_feautres_extractor(std::string const &model_params,
                              std::string const &model_symbols,
                              mxnet::cpp::Context const &context);
    ~person_feautres_extractor();

    cv::Mat_<float> get_features(cv::Mat const &input);

private:
    std::unique_ptr<mxnet::cpp::Executor> executor_;    
    std::unique_ptr<ocv::img_cvt::opencv_to_ndarray> to_ndarray_;
};

#endif // PERSON_FEAUTRES_EXTRACTOR_HPP
