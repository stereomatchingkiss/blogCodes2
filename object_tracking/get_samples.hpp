#ifndef GET_SAMPLES_HPP
#define GET_SAMPLES_HPP

#include <opencv2/core.hpp>

#include <string>
#include <vector>

/**
 * Get usc pedestrian.The organization of the data are
 * folder/(images)
 * folder/GT/(ground truth)
 * @param folders folders of usc
 * @return pedestrian of usc dataset
 */
std::vector<cv::Mat>
get_usc_pedestrian(std::vector<std::string> folders);

#endif //GET_SAMPLES_HPP
