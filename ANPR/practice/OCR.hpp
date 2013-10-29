#ifndef OCR_HPP
#define OCR_HPP

#include <string>
#include <vector>

#include <opencv2/core/core.hpp>
#include <opencv2/ml/ml.hpp>

#include "plate.hpp"

class OCR
{

    using ContourType = std::vector<std::vector<cv::Point>>;
public:
    //OCR(std::string trainFile);
    OCR();

    cv::Mat features(cv::Mat const &input, int sizeData) const;
    //std::string run(plate *input);
    cv::Mat shiftToCenter(cv::Mat const &in);
    //int classify(cv::Mat f);
    void train(cv::Mat const &trainData, cv::Mat const &classes, int nlayers, int numCharacters);
    //int classifyKnn(cv::Mat f);
    //void trainKnn(cv::Mat trainSamples, cv::Mat trainClasses, int k);    

public:
    int charSize;
    bool debug;
    std::string filename;
    bool saveSegments;
    bool trained;

private:
    class CharSegment
    {
    public:
        CharSegment() = default;
        CharSegment(cv::Mat &i, cv::Rect const &p);
        cv::Mat img;
        cv::Rect pos;
    };

private:    
    cv::Mat preprocess(cv::Mat const &in, int newSize);
    //cv::Mat getVisualHistogram(cv::Mat *hist, int type);
    //void drawVisualFeatures(cv::Mat character, cv::Mat hhist, cv::Mat vhist, cv::Mat lowData);
    cv::Mat projectedHistogram(cv::Mat const &img, bool transpose) const;
    void segment(plate &input);
    bool verifySizes(cv::Mat const &r) const;

private:
    CvANN_MLP  ann;
    std::vector<OCR::CharSegment> charSegments;
    cv::Mat img_contours;
    int K;
    CvKNearest knnClassifier;

    cv::Mat img_threshold;
};

#endif // OCR_HPP
