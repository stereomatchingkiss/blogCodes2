#ifndef KMEANSSEGMENT_HPP
#define KMEANSSEGMENT_HPP

#include <opencv2/core/core.hpp>

/**
 * @brief simplify the procedures of using cv::kmeans to do color segmentation
 */
class kmeansSegment
{
public:
    explicit kmeansSegment(int cluster_number, cv::TermCriteria criteria = cv::TermCriteria(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 10, 0.01),
                           int attempts = 5, int flags = cv::KMEANS_PP_CENTERS);

    cv::Mat segment(cv::Mat const &input);
    cv::Mat segment(cv::Mat const &input, int cluster_number, cv::TermCriteria criteria, int attempts, int flags);

    void set_attempts(int attempts) { attempts_ = attempts; }
    void set_cluster_number(int cluster_number) { cluster_number_ = cluster_number; }
    void set_criteria(cv::TermCriteria criteria) { criteria_ = criteria; }
    void set_flags(int flags) { flags_ = flags; }

private:
    void kmeans_result(cv::Mat &inout, cv::Mat const &labels, cv::Mat const &centers);
    void kmeans_sample(cv::Mat const &input, cv::Mat &output);

private:
    //all of the meaning of the parameters can get from
    //the doc of openCV
    int attempts_;
    cv::Mat centers_;
    int cluster_number_;
    cv::TermCriteria criteria_;
    int flags_;
    cv::Mat labels_;
    cv::Mat results_;
    cv::Mat samples_;
};

#endif // KMEANSSEGMENT_HPP
