#ifndef RE_ID_DB_HPP
#define RE_ID_DB_HPP

#include <opencv2/core.hpp>

#include <memory>
#include <string>
#include <vector>

namespace mxnet{

namespace cpp{

class Context;
class Executor;
class NDArray;

}

}

namespace mxnet_aux{

class opencv_to_ndarray;

}

class cosine_similarity;

class re_id_db
{
public:
    struct id_info
    {
        float confident_ = -2.0f;
        size_t id_ = 0;
    };

    re_id_db(std::string const &model_params,
             std::string const &model_symbols,
             mxnet::cpp::Context const &context);
    ~re_id_db();

    size_t add_new_id(cv::Mat const &input);
    id_info find_most_similar_id(cv::Mat const &input) const;

private:
    std::unique_ptr<cosine_similarity> compare_;
    std::unique_ptr<mxnet::cpp::Executor> executor_;
    std::vector<cv::Mat_<float>> features_;
    std::unique_ptr<mxnet_aux::opencv_to_ndarray> to_ndarray_;
};

#endif // RE_ID_DB_HPP
