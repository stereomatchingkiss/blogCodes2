#include "montage.hpp"

#include <opencv2/imgproc.hpp>

namespace ocv{

montage::montage(cv::Size const &size,
                 int grid_x, int grid_y, int interpolation) :
    grid_x_(grid_x),
    grid_y_(grid_y),
    interpolation_(interpolation),
    size_(size)
{
    CV_Assert(grid_x > 0 && grid_y > 0);
}

void montage::add_image(cv::Mat const &input)
{
    if(montages_.size() < static_cast<size_t>(grid_x_ * grid_y_)){
        montages_.emplace_back(input);
    }else{
        montages_.emplace(std::begin(montages_), input);
        montages_.pop_back();
    }
}

void montage::clear()
{
    montages_.clear();
}

cv::Mat montage::get_montage() const
{
    if(!montages_.empty()){
        cv::Mat dst(size_.height * grid_y_,
                    size_.width * grid_x_,
                    montages_[0].type());        
        tile(montages_, dst, grid_x_, grid_y_);

        return dst;
    }

    return {};
}

void montage::tile(const std::vector<cv::Mat> &src,
                   cv::Mat &dst, int grid_x, int grid_y) const
{
    // patch size
    int const width  = dst.cols/grid_x;
    int const height = dst.rows/grid_y;    
    // iterate through grid
    int k = 0;
    for(int i = 0; i < grid_y; i++) {
        for(int j = 0; j < grid_x; j++) {
            cv::Mat s = src[k++];            
            cv::resize(s,s, cv::Size(width,height), 0, 0, interpolation_);
            s.copyTo(dst(cv::Rect(j*width,i*height,width,height)));
        }
    }
}

}
