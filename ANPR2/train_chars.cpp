#include "train_chars.hpp"

#include <ocv_libs/file/utility.hpp>
#include <ocv_libs/ml/utility/split_train_test.hpp>

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <iostream>
#include <random>

train_chars::train_chars(std::string chars_folder,
                         std::string result_folder) :
    bbps_({30, 15}, {{5,5}, {5,10}, {10,5}, {10,10}}),
    chars_folder_(std::move(chars_folder)),
    result_folder_(std::move(result_folder))
{
    using namespace boost::filesystem;

    generate_train_number();

    if(!boost::filesystem::exists(path(result_folder_))){
        create_directory(path(result_folder_));
    }

    int index = 0;
    for(char i = '0'; i <= '9'; ++i){
        bm_labels_int_.insert({std::string(1,i), index++});
    }
    for(char i = 'a'; i <= 'z'; ++i){
        bm_labels_int_.insert({std::string(1,i), index++});
    }
}

void train_chars::extract_features()
{
    auto const folders =
            ocv::file::get_directory_folders(chars_folder_);

    std::random_device rd;
    std::mt19937 g(rd());
    for(size_t i = 0; i != folders.size(); ++i){
        auto const folder = chars_folder_ + "/" + folders[i];
        std::cout<<folder<<std::endl;

        auto it = bm_labels_int_.left.find(folders[i]);
        if(it != bm_labels_int_.left.end()){
            int const label = it->second;
            auto files = ocv::file::get_directory_files(folder);
            std::shuffle(std::begin(files), std::end(files), g);
            files.resize(train_size_ + validate_size_);

            for(size_t j = 0; j != files.size(); ++j){
                std::cout<<folder + "/" + files[j]<<std::endl;
                auto img = cv::imread(folder + "/" + files[j]);
                if(!img.empty()){
                    std::cout<<"can read file"<<std::endl;
                    auto feature = bbps_.describe(img);
                    features_.emplace_back(std::move(feature));
                    labels_.emplace_back(label);
                }
            }
        }else{
            std::cout<<"cannot find the value of key "<<std::endl;
        }
    }
}

void train_chars::split_train_test()
{
    using namespace ocv::ml;
    auto results = split_train_test_inplace(features_,labels_,0.1,false);
    features_.clear();
    features_train_.resize(0);
    features_validate_.resize(0);
    labels_.clear();
    labels_train_.clear();
    labels_validate_.clear();
    for(size_t i = 0; i != std::get<0>(results).size(); ++i){
        features_train_.push_back(cv::Mat(std::get<0>(results)[i], true));
        labels_train_.emplace_back(std::get<1>(results)[i]);
    }
    for(size_t i = 0; i != std::get<2>(results).size(); ++i){
        features_validate_.push_back(cv::Mat(std::get<2>(results)[i], true));
        labels_validate_.emplace_back(std::get<3>(results)[i]);
    }
}

void train_chars::describe_features()
{
    extract_features();
    split_train_test();
}

void train_chars::train_svm()
{
    using namespace cv::ml;

    auto svm = cv::ml::SVM::create();
    svm->setType(SVM::C_SVC);
    svm->setKernel(SVM::LINEAR);
    svm->setTermCriteria(cv::TermCriteria(cv::TermCriteria::MAX_ITER,
                                          100, 1e-6));

    auto train_data = TrainData::create(features_train_.reshape(1, int(labels_train_.size())),
                                        ROW_SAMPLE,
                                        labels_train_);
    svm->trainAuto(train_data);
    std::cout<<"c : "<<svm->getC()<<"\n";
    std::cout<<"coef0 : "<<svm->getCoef0()<<"\n";
    std::cout<<"degree : "<<svm->getDegree()<<"\n";
    std::cout<<"gamma : "<<svm->getGamma()<<"\n";

    svm->write(cv::FileStorage("chars_classifier.xml",
                               cv::FileStorage::WRITE));

    ml_ = svm;
}

void train_chars::generate_train_number()
{
    if(!boost::filesystem::exists(chars_folder_)){
        std::cout<<"chars folder do not exist"<<std::endl;
        return;
    }

    auto folders = ocv::file::get_directory_folders(chars_folder_);
    size_t min = std::numeric_limits<size_t>::max();
    for(size_t i = 0; i != folders.size(); ++i){
        auto const folder = chars_folder_ + "/" + folders[i];
        auto const img_size = ocv::file::get_directory_files(folder).size();
        std::cout<<folder<<" has "<<img_size<<" images"<<std::endl;
        if(img_size < min){
            min = img_size;
        }
    }
    train_size_ = static_cast<size_t>(min * 0.8);
    validate_size_ = min - train_size_;
    std::cout<<"min images size is "<<min<<std::endl;
}

cv::Ptr<cv::ml::StatModel> train_chars::train()
{
    describe_features();
    train_svm();

    return ml_;
}
