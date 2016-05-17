#ifndef GET_SAMPLES_HPP
#define GET_SAMPLES_HPP

#include <opencv2/core.hpp>

#include <functional>
#include <string>
#include <vector>

/**
 * Read the pose of stanford40
 * @param folders folders of stanford40
 * @param extract_size number of sample retrieve, if the
 * size larger than the dataset, this function will
 * retrieve whole dataset
 * @param preprocess preprocess the image
 * @param seed seed for random shuffle
 * @return poses of stanford40
 * @code
 * * std::string const prefix();
 * get_usc_pedestrian("../computer_vision_dataset/
 * human_pose/standord40");
 * @endcode
 */
std::vector<cv::Mat>
get_stanford40_pose(std::string const &folders,
                    size_t extract_size,
                    std::function<void(cv::Mat&)> preprocess,
                    unsigned int seed = 0);

/**
 * Get usc pedestrian.The organization of the data are
 * folder/(images)
 * folder/GT/(ground truth)
 * @param folders folders of usc
 * @param extract_size number of sample retrieve, if the
 * size larger than the dataset, this function will
 * retrieve whole dataset
 * @param preprocess preprocess the image
 * @param seed seed for random shuffle
 * @return pedestrian of usc dataset
 * @code
 * std::string const prefix("../computer_vision_dataset/
 * pedestrian");
 * get_usc_pedestrian(prefix + "/USCPedestrianSetA")
 * @endcode
 */
std::vector<cv::Mat>
get_usc_pedestrian(std::string const &folders,
                   size_t extract_size,
                   std::function<void(cv::Mat&)> preprocess,
                   unsigned int seed = 0);

#endif //GET_SAMPLES_HPP
