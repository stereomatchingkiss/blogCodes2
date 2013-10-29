#include <algorithm>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <basicImageAlgo.hpp>

#include "OCR.hpp"

namespace
{

enum class Orientation : bool { VERTICAL, HORIZONTAL };

}

OCR::CharSegment::CharSegment(cv::Mat &i, cv::Rect const &p) : img(i), pos(p)
{}

OCR::OCR() : debug{false}, trained{false}
{
}

cv::Mat OCR::features(cv::Mat const &input, int sizeData) const
{
    //Histogram features
    cv::Mat vhist = projectedHistogram(input, static_cast<bool>(Orientation::VERTICAL));
    cv::Mat hhist = projectedHistogram(input, static_cast<bool>(Orientation::HORIZONTAL));

    //Low data feature
    cv::Mat lowData;
    cv::resize(input, lowData, cv::Size(sizeData, sizeData));

    //if(DEBUG)
    //    drawVisualFeatures(in, hhist, vhist, lowData);

    //Last 10 is the number of moments components
    int numCols = vhist.cols + hhist.cols + lowData.cols * lowData.cols;

    cv::Mat out = cv::Mat::zeros(1, numCols, CV_32F);
    //Asign values to feature
    auto out_ptr = out.ptr<float>(0);
    OCV::for_each_channels<float>(vhist, [&](float a)
    {
        *out_ptr = a;
        ++out_ptr;
    });
    OCV::for_each_channels<float>(hhist, [&](float a)
    {
        *out_ptr = a;
        ++out_ptr;
    });
    OCV::for_each_channels<uchar>(lowData, [&](uchar a)
    {
        *out_ptr = a;
        ++out_ptr;
    });

    //if(debug)
    //    std::cout << out << "\n===========================================\n";
    return out;
}

/**
 * @brief Remap input to center of the output
 * @param input input image
 * @return
 */
cv::Mat OCR::shiftToCenter(cv::Mat const &input)
{
    int const h = input.rows;
    int const w = input.cols;
    cv::Mat transformMat = cv::Mat::eye(2, 3, CV_32F);
    int const maxSide = std::max(w, h);
    transformMat.at<float>(0,2) = maxSide/2 - w/2;
    transformMat.at<float>(1,2) = maxSide/2 - h/2;

    cv::Mat warpImage(maxSide, maxSide, input.type());
    cv::warpAffine(input, warpImage, transformMat, warpImage.size(), cv::INTER_LINEAR, cv::BORDER_CONSTANT, cv::Scalar(0) );

    cv::Mat out;
    cv::resize(warpImage, out, cv::Size(charSize, charSize));

    return out;
}

void OCR::train(cv::Mat const &trainData, cv::Mat const &classes, int nlayers, int numCharacters)
{
    int buffer[] = {trainData.cols, nlayers, numCharacters};
    cv::Mat const layers(1, 3, CV_32SC1, buffer);
    ann.create(layers, CvANN_MLP::SIGMOID_SYM, 1, 1);

    //Prepare trainClases
    //Create a mat with n trained data by m classes
    cv:: Mat trainClasses;
    trainClasses.create(trainData.rows, numCharacters, CV_32FC1);
    for( int i = 0; i !=  trainClasses.rows; ++i){
        int const labels = *classes.ptr<int>(i);
        auto train_ptr = trainClasses.ptr<float>(i);
        for(int k = 0; k != trainClasses.cols; ++k){
            *train_ptr = k != labels ? 0 : 1;
            ++train_ptr;
        }
    }

    cv::Mat const weights = cv::Mat::ones(1, trainData.rows, CV_32FC1);

    //Learn classifier
    ann.train(trainData, trainClasses, weights);
}

/**********************************************************
 ****************** implementation ************************
 **********************************************************/

/*cv::Mat OCR::preprocess(cv::Mat const &in, int newSize)
{

}*/

/**
 * @brief  OCR::projectedHistogram
 * @param  img
 * @param  transpose : true : projected horizontal histogram; false : projected vertical histogram
 * @return histogram after projected
 */
cv::Mat OCR::projectedHistogram(cv::Mat const &img, bool transpose) const
{
    int const sz = (transpose) ? img.rows:img.cols;
    cv::Mat mhist = cv::Mat::zeros(1, sz, CV_32F);
    auto mhist_ptr = mhist.ptr<float>(0);

    if(transpose){
        for(int j = 0; j != sz; ++j){
            *mhist_ptr = cv::countNonZero(img.row(j));
            ++mhist_ptr;
        }
    }else{
        for(int j = 0; j != sz; ++j){
            *mhist_ptr = cv::countNonZero(img.col(j));
            ++mhist_ptr;
        }
    }

    //Normalize histogram
    double min, max;
    cv::minMaxLoc(mhist, &min, &max);

    if(max > 0){
        //mhist.convertTo(mhist, -1, 1.0f / max, 0);
        mhist /= max;
    }

    return mhist;
}

void OCR::segment(plate &input)
{
    cv::Mat inputPlate = input.plateImg;
    charSegments.clear();

    cv::threshold(inputPlate, img_threshold, 0, 255, cv::THRESH_BINARY_INV + cv::THRESH_OTSU);
    if(debug){
        cv::imshow("threshold", img_threshold);
        cv::waitKey();
    }

    img_threshold.copyTo(img_contours);
    ContourType contours;
    cv::findContours(img_contours, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

    // Draw blue contours on a white image
    cv::Mat result;
    img_threshold.copyTo(result);
    cvtColor(result, result, CV_GRAY2BGR);
    cv::drawContours(result, contours,
                     -1, // draw all contours
                     cv::Scalar(255,0,0), // in blue
                     1); // with a thickness of 1

    for(auto const &data : contours){
        //Create bounding rect of object
        cv::Rect const mr = cv::boundingRect(data);
        cv::rectangle(result, mr, cv::Scalar(0, 255, 0));
        //Crop image
        cv::Mat const auxRoi = img_threshold(mr);
        if(verifySizes(auxRoi)){
            cv::Mat output = shiftToCenter(auxRoi);
            charSegments.emplace_back(output, mr);
            cv::rectangle(result, mr, cv::Scalar(0, 125, 255));
        }
    }
}

bool OCR::verifySizes(cv::Mat const &r) const
{
    //Char sizes 45x77
    float const aspect = 45.0f / 77.0f;
    float const charAspect = (float)r.cols/(float)r.rows;
    float const error = 0.35;
    float const minHeight = 15;
    float const maxHeight = 28;
    //We have a different aspect ratio for number 1, and it can be ~0.2
    float const minAspect = 0.2;
    float const maxAspect = aspect + aspect * error;
    //area of pixels
    float const area = cv::countNonZero(r);
    //bb area
    float const bbArea = r.cols * r.rows;
    //% of pixel in area
    float const percPixels = area / bbArea;

    if(debug){
        std::cout << "Aspect: "<< aspect << " ["<< minAspect << "," << maxAspect << "] "  << "Area "<< percPixels
                  <<" Char aspect " << charAspect  << " Height char "<< r.rows << "\n";
    }

    if(percPixels < 0.8 && charAspect > minAspect && charAspect < maxAspect && r.rows >= minHeight && r.rows < maxHeight){
        return true;
    }else{
        return false;
    }
}
