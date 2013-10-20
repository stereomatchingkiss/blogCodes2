#include "kmeansSegment.hpp"

kmeansSegment::kmeansSegment(int cluster_number, cv::TermCriteria criteria, int attempts, int flags)
    : attempts_(attempts), cluster_number_(cluster_number), criteria_(criteria), flags_(flags)
{
}

/**
 * @brief do color segmentation by cv::kmeans
 * @param input input image
 * @return the result, it is sharing the data of the class
 */
cv::Mat kmeansSegment::segment(cv::Mat const &input)
{    
    kmeans_sample(input, samples_);

    cv::kmeans(samples_, cluster_number_, labels_, criteria_, attempts_, flags_, centers_);

    results_.create(input.size(), input.type());
    kmeans_result(results_, labels_, centers_);

    return results_;
}

/**
 * @brief do color segmentation by cv::kmeans
 * @param input           input image
 * @param cluster_number  Number of clusters to split the set by
 * @param criteria        The algorithm termination criteria, that is, the maximum number
 * of iterations and/or the desired accuracy. The accuracy is specified as criteria.epsilon.
 * As soon as each of the cluster centers moves by less than criteria.epsilon on some
 * iteration, the algorithm stops.
 * @param attempts        Flag to specify the number of times the algorithm is executed
 * using different initial labellings. The algorithm returns the labels that yield the
 * best compactness (see the last function parameter).
 * @param flags           Flag that can take the following values:

    KMEANS_RANDOM_CENTERS Select random initial centers in each attempt.
    KMEANS_PP_CENTERS Use kmeans++ center initialization by Arthur and Vassilvitskii [Arthur2007].
    KMEANS_USE_INITIAL_LABELS During the first (and possibly the only) attempt,
    use the user-supplied labels instead of computing them from the initial centers.
    For the second and further attempts, use the random or semi-random centers.
    Use one of KMEANS_*_CENTERS flag to specify the exact method.

 * @return the result, it is sharing the data of the class
 */
cv::Mat kmeansSegment::segment(cv::Mat const &input, int cluster_number, cv::TermCriteria criteria, int attempts, int flags)
{
    attempts_ = attempts;
    cluster_number_ = cluster_number;
    criteria_ = criteria;
    flags_ = flags;
    segment(input);

    return results_;
}

/**********************************************************
 ****************** implementation ************************
 **********************************************************/

/**
 * @brief generate the quantization results of cv::kmeans into an image
 * @param inout   input and output, the memory should be allocated before pass in
 * @param labels  Input/output integer array that stores the cluster indices for every sample.
 * @param centers matrix of the cluster centers, one row per each cluster center.
 */
void kmeansSegment::kmeans_result(cv::Mat &inout, cv::Mat const &labels, cv::Mat const &centers)
{
    for( int row = 0; row != inout.rows; ++row){
        auto inout_ptr = inout.ptr<uchar>(row);
        auto labels_ptr = labels.ptr<int>(row * inout.cols);
        for( int col = 0; col != inout.cols; ++col){
            int const cluster_idx = labels_ptr[col];
            auto centers_ptr = centers.ptr<float>(cluster_idx);
            for(int ch = 0; ch != inout.channels(); ++ch){
                inout_ptr[ch] = centers_ptr[ch];
            }
            inout_ptr += inout.channels();
        }
    }
}

/**
 * @brief generate kmeans sample needed by cv::kmenas, only suit for two dimensions images
 * @param input   input image
 * @param output  kmeans sample
 */
void kmeansSegment::kmeans_sample(cv::Mat const &input, cv::Mat &output)
{
    if(output.type() != CV_32F || output.rows != static_cast<int>(input.total()) || output.channels() != input.channels()){
        output.create(input.total(), input.channels(), CV_32F);
        output.reshape(0); //make sure it is continous
    }

    auto output_ptr = output.ptr<float>(0);
    for( int row = 0; row != input.rows; ++row){
        auto input_ptr = input.ptr<uchar>(row);        
        for( int col = 0; col != input.cols; ++col){
            for( int ch = 0; ch != input.channels(); ++ch){
                *output_ptr = *input_ptr;
                ++output_ptr; ++input_ptr;
            }
        }
    }
}
