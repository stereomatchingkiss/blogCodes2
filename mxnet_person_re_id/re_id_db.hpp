#ifndef RE_ID_DB_HPP
#define RE_ID_DB_HPP

#include <opencv2/core.hpp>

#include <memory>
#include <string>
#include <vector>

class cosine_similarity;

class re_id_db
{
public:
    struct id_info
    {
        float confident_ = -2.0f;        
        size_t id_ = 0;
    };

    re_id_db();
    ~re_id_db();

    size_t add_new_id(cv::Mat_<float> const &input);
    id_info find_most_similar_id(cv::Mat_<float> const &input);

private:
    std::unique_ptr<cosine_similarity> compare_;    
    std::vector<cv::Mat_<float>> features_;    
};

#endif // RE_ID_DB_HPP
