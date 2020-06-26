#ifndef OCV_CORE_MONTAGE_HPP
#define OCV_CORE_MONTAGE_HPP

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <vector>

/*!
 *  \addtogroup ocv
 *  @{
 */
namespace ocv{

/**
 * create montage of images
 * @code
 * montage mt({64,64}, 4, 4);
 * mt.add_image(img_0);
 * //.........
 * mt.add_image(img_15);
 * imshow("mt",mt.get_montage());
 * waitKey();
 * @endcode
 */
class montage
{
public:
    /**
     * Constructor
     * @param size size of the image of the montage
     * @param grid_x number of images on x axis
     * @param grid_y number of images on y axis
     * @param interpolation interpolation method, see cv::InterpolationFlags
     */
    montage(cv::Size const &size, int grid_x, int grid_y, int interpolation = cv::INTER_LINEAR);

    /**
     * Add image into the montage, if the size of the montage
     * greater than gird_x * grid_y, it will pop out the
     * first image insert into the montage
     * @param input image want to add into the montage
     */
    void add_image(cv::Mat const &input);
    /**
     * clear montage
     */
    void clear();
    /**
     * get montage
     * @return montage
     */
    cv::Mat get_montage() const;

private:
    void tile(std::vector<cv::Mat> const &src, cv::Mat &dst,
              int grid_x, int grid_y) const;

    int grid_x_;
    int grid_y_;
    int interpolation_;
    std::vector<cv::Mat> montages_;
    cv::Size size_;
};

} /*! @} End of Doxygen Groups*/

#endif // OCV_CORE_MONTAGE_HPP
