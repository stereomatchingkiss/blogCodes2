#ifndef PLAYER_DETECTOR_HPP
#define PLAYER_DETECTOR_HPP

#include <opencv2/core.hpp>
#include <opencv2/bgsegm.hpp>

class player_detector
{
public:
    explicit player_detector(size_t max_player = 1);

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
     * some algorithms need to warm_up, like MOG2 of opencv
     * @param input input image
     */
    void warm_up(cv::Mat const &input);

private:
   cv::Ptr<cv::BackgroundSubtractor> bgs_;
   std::vector<std::vector<cv::Point>> contours_;
   cv::Mat gray_img_;
   cv::Mat fmask_;
   size_t max_player_;
};

#endif
