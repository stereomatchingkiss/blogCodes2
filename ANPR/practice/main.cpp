#include <array>
#include <complex>
#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/ml/ml.hpp>

#include <basicImageAlgo.hpp>

#include "detectRegions.hpp"
#include "OCR.hpp"
#include "plate.hpp"

std::string const prefix("/Users/Qt/program/blogsCodes/pic/images/ANPR/");
std::string const names[] = {"9773BNB.jpg", "DSC_0562.jpg", "DSC_0566.jpg",
                             "2715DTZ.jpg", "3028BYS.JPG", "3154FFY.JPG",
                             "3266CNT.JPG", "3732FWW.JPG", "5445BSX.JPG",
                             "7215BGN.JPG", "9588DWV.jpg", "8995CCN.JPG"};

size_t const SIZE = sizeof(names) / sizeof(std::string);

inline void mat_data(cv::Mat const &data)
{
    std::cout<<"rows = "<<data.rows<<", cols = "<<data.cols<<", channels = "<<data.channels()<<std::endl;
}

void showSingle(size_t index)
{
    cv::Mat input = cv::imread(prefix + names[index]);
    if(input.empty()){
        std::cerr<<"can't read image : "<<names[index]<<std::endl;
        return;
    }

    detectRegions region;
    region.run(input);
}

void showAll()
{
    for(size_t i = 0; i != SIZE; ++i){
        showSingle(i);
    }
}

template<typename T = uchar, typename U = float>
void transform_to_svm_training_data(cv::Mat &input)
{
    if(input.isContinuous()){
        input = input.reshape(1, 1);
        input.convertTo(input, cv::DataType<U>().depth);
        return;
    }

    cv::Mat output(1, input.total() * input.channels(), cv::DataType<U>().depth);
    auto output_ptr = output.ptr<U>(0);
    OCV::for_each_channels<T>(input, [&](T a)
    {
        *output_ptr = a;
        ++output_ptr;
    });

    input = output;
}

void test_SVM(size_t index)
{    
    cv::FileStorage fs("/Users/Qt/program/blogsCodes/ANPR/practice/SVM.xml", cv::FileStorage::READ);
    cv::Mat trainingData;
    cv::Mat classes;
    fs["TrainingData"] >> trainingData;
    fs["classes"] >> classes;

    CvSVMParams SVM_params;
    SVM_params.svm_type = CvSVM::C_SVC;
    SVM_params.kernel_type = CvSVM::LINEAR;
    SVM_params.degree = 0;
    SVM_params.gamma = 1;
    SVM_params.coef0 = 0;
    SVM_params.C = 1;
    SVM_params.nu = 0;
    SVM_params.p = 0;
    SVM_params.term_crit = cv::TermCriteria(CV_TERMCRIT_ITER, 1000, 0.01);

    //Train SVM
    cv::SVM svmClassifier;
    svmClassifier.train(trainingData, classes, cv::Mat(), cv::Mat(), SVM_params);

    cv::Mat input = cv::imread(prefix + names[index]);
    if(input.empty()){
        std::cerr<<"can't read image : "<<names[index]<<std::endl;
        return;
    }

    detectRegions region;
    std::vector<plate> possible_plates = region.run(input);
    std::vector<plate> plates;
    for(auto &plate : possible_plates){
        cv::Mat img = plate.plateImg;
        transform_to_svm_training_data<uchar, uchar>(img);
        img.convertTo(img, CV_32F);
        int const response = static_cast<int>(svmClassifier.predict(img));
        if(response == 1){
            plates.emplace_back(std::move(plate));
        }
    }

    std::cout<<"num plated detected "<<plates.size()<<std::endl;
    for(auto &plate : plates){
        cv::imshow("plate", plate.plateImg);
        cv::waitKey();
    }
}

void test_SVM_all()
{
    for(size_t i = 0; i != SIZE; ++i){
        test_SVM(i);
    }
}

int main()
{
    //showAll();
    //showSingle(0);

    return 0;
}
