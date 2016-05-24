#ifndef TINY_CNN_HUMAN_DETECTOR_HPP
#define TINY_CNN_HUMAN_DETECTOR_HPP

#include <opencv2/core.hpp>

#include <tiny_cnn/tiny_cnn.h>

#include <ocv_libs/tiny_cnn/predictor.hpp>

#include <functional>

class tiny_cnn_human_detector
{
public:
    //first is probability of human
    using result_type =
    std::vector<cv::Rect2d>;

    explicit tiny_cnn_human_detector(size_t max_player = 1);

    /**
     * classify input is a human or not
     * @param input should be CV_8UC3 or CV_8U
     * @return true if it is human;else false
     */
    bool is_human(cv::Mat const &input);

    /**
     * search the player without the constraint with max player
     * @param input input image
     * @return locations of players
     */
    std::vector<cv::Rect2d> search_simple(cv::Mat const &input);
    /**
     * search the player with the constraint with max player
     * @param input input image
     * @return locations of players
     */
    std::vector<cv::Rect2d> search(cv::Mat const &input);

    /**
     * min area of the human, if the bounding rect less than
     * min area, it will not treat as human.
     * @param value value of min area
     */
    void set_min_area(double value);

    /**
     * set the threshold of "is human" probability
     * @param threshold self explain
     */
    void set_threshold(double threshold);

    /**
     * open/close the debug message
     * @param value true, open debug message and vice versa
     */
    void set_verbose(bool value);

    /**
     * do nothing, only for api consistency
     */
    void warm_up(cv::Mat const&){}

private:
    using NetType =
    tiny_cnn::network<tiny_cnn::mse, tiny_cnn::adam>;

    NetType create_network();
    /**
     * predict input is a human or not
     * @param input should be CV_8UC3 or CV_8U
     * @return true if it is human;else false
     */
    bool is_human(cv::Mat const &input, bool do_preprocess);
    void preprocess(cv::Mat const &input);

    tiny_cnn::vec_t candiate_img_;
    cv::Mat candidate_mat_;

    cv::Mat gray_img_;    

    size_t max_player_;
    double min_area_;

    ocv::tcnn::predictor<NetType> predictor_;

    double threshold_;
    bool verbose_;
};

#endif
