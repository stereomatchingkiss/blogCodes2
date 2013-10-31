#include <iostream>
#include <string>

#include <opencv2/highgui/highgui.hpp>

#include <basicImageAlgo.hpp>
#include <utility/utility.hpp>

#include "characterRecognizer.hpp"
#include "detectCharacter.hpp"

/**
 * @brief this is a self learning project, the purpose is study the
 *  how to train and detect the digits by ANN(cv::)
 */

void write_digits_xml(std::vector<cv::Mat> &training_data, std::vector<int> const &training_labels)
{
    cv::Mat train_data;
    cv::Mat train_labels;

    for(auto &data : training_data){
        OCV::transform_to_svm_training_data(data);
        train_data.push_back(data);
    }

    cv::Mat(training_labels).copyTo(train_labels);

    cv::FileStorage fs("digits.xml", cv::FileStorage::WRITE);
    fs << "TrainingData10x10" << train_data;
    fs << "Labels" << train_labels;
}

int main()
{   
    try{

        //step 1 : segment each digit into the vector
        std::string const prefix("/Users/Qt/program/blogsCodes/");
        cv::Mat img = cv::imread(prefix + "pic/digits00.png");
        if(img.empty()){
            std::cerr<<"can't read image"<<std::endl;
            return - 1;
        }

        detectCharacter dc;
        std::vector<cv::Mat> training_data = dc.segment(img, {10, 10});

        //step 2 : set up training labels
        std::vector<int> const training_labels
        {
            7, 4, 8, 2, 1, 6, 4, 4, 3, 3, 9, 5, 6, 6, 5,
            7, 9, 0, 1, 8, 8, 2, 4, 4, 6, 9, 1, 8, 3, 0,
            3, 9, 4, 5, 9, 8, 4, 9, 2, 2, 6, 4, 4, 6, 9,
            5, 5, 5, 0, 1, 1, 2, 5, 8, 3, 9, 1, 0, 7, 2,
            0, 1, 4, 8, 2, 0, 5, 4, 7, 1, 1, 1, 8, 4, 8,
            2, 1, 8, 0, 4, 9, 5, 3, 5, 2, 7, 1, 3, 2, 2,
            8, 5, 0, 5, 5, 9, 0, 6, 4, 4, 8, 3, 9, 0, 7,
            4, 6, 6, 0, 3, 2, 8, 2, 3, 1, 5, 6, 8, 0, 8,
            4, 1, 2, 8, 9
        };

        //step 3 : write the training data and training labels to xml
        write_digits_xml(training_data, training_labels);

        //ste4 : segment the digits of the target image
        cv::Mat target = cv::imread(prefix + "pic/digitTarget00.png");
        if(target.empty()){
            std::cerr<<"can't read target"<<std::endl;
            return -1;
        }

        std::vector<cv::Mat> target_digits = dc.segment(target, {10, 10});
        std::vector<cv::Mat> temp_target_digits; //for verification
        for(auto &data : target_digits){
            temp_target_digits.emplace_back(data.clone());
            OCV::transform_to_svm_training_data(data);
        }

        //step 5 : train and classify the digits
        characterRecognizer cr(prefix + "simpleOCR/build-exercise00-Qt5_1_1_clang_3_2-Release/digits.xml",
                               "TrainingData10x10", "Labels");
        cr.train(10, 10);

        for(size_t i = 0, size = target_digits.size(); i != size; ++i){
            int const classify = cr.classify(target_digits[i]);
            std::cout<<classify<<std::endl;
            cv::imshow("", temp_target_digits[i]);
            cv::waitKey();
        }

    }catch(std::exception const &ex){
        std::cerr<<ex.what()<<std::endl;
    }

    return 0;
}
