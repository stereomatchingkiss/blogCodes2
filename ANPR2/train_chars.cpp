#include "train_chars.hpp"

#include <ocv_libs/file/utility.hpp>

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <boost/filesystem.hpp>

#include <iostream>
#include <random>

train_chars::train_chars(std::string chars_folder,
                         std::string result_folder,
                         map_type mtype) :
    chars_folder_(std::move(chars_folder)),
    generator_(std::random_device()()),
    mtype_{mtype},
    result_file_(std::move(result_folder))
{
    using namespace boost::filesystem;

    if(!exists(path(result_file_))){
        create_directory(path(result_file_));
    }

    generate_map(bm_, mtype_);
    generate_train_number();
}

void train_chars::extract_feature(cv::Mat const &img, int label)
{
    auto feature = bbps_.describe(binarize_image(img));
    features_.emplace_back(std::move(feature));
    labels_.emplace_back(label);
}

void train_chars::extract_features()
{
    auto const folders =
            ocv::file::get_directory_folders(chars_folder_);

    features_.clear();
    features_train_.resize(0);
    labels_.clear();
    for(size_t i = 0; i != folders.size(); ++i){
        auto const folder = chars_folder_ + "/" + folders[i];
        auto it = bm_.left.find(folders[i]);
        if(it != std::end(bm_.left)){
            int const label = it->second;
            std::cout<<__func__<<", label = "<<label<<std::endl;
            auto files = ocv::file::get_directory_files(folder);
            std::shuffle(std::begin(files), std::end(files), generator_);

            for(size_t j = 0; j != min_symbol_size_; ++j){
                auto img = cv::imread(folder + "/" + files[j]);
                if(!img.empty()){
                    extract_feature(img, label);
                    add_noise(img, label);
                    add_rotation(img, label, 10);
                    add_rotation(img, label, -10);
                }
            }
        }
    }
    std::cout<<"extract labels size = "<<labels_.size()<<std::endl;
}

void train_chars::split_train_test()
{
    for(size_t i = 0; i != labels_.size(); ++i){
        features_train_.push_back(cv::Mat(features_[i], false));
        labels_train_.emplace_back(labels_[i]);
    }
}

void train_chars::test_train_result()
{    
    std::cout<<"---------train precision---------------"<<std::endl;
    show_training_results(features_, labels_train_);
}

void train_chars::describe_features()
{
    extract_features();
    split_train_test();
}

void train_chars::train_classifier()
{
    using namespace cv::ml;

    std::cout<<__func__<<std::endl;
    /*auto ml = cv::ml::RTrees::create();
    ml->setMaxDepth(20);
    ml->setMinSampleCount(static_cast<int>(labels_train_.size() * 0.01));
    ml->setRegressionAccuracy(0);
    ml->setUseSurrogates(false);
    ml->setMaxCategories(10);
    ml->setPriors(cv::Mat());
    ml->setCalculateVarImportance(false);
    ml->setActiveVarCount(0);
    ml->setTermCriteria({cv::TermCriteria::MAX_ITER,
                         10000, 1e-6});

    auto train_data = TrainData::create(features_train_.reshape(1, int(labels_train_.size())),
                                        ROW_SAMPLE,
                                        labels_train_);

    ml->train(train_data);
    std::cout<<__func__<<" : finish train"<<std::endl;//*/

    auto ml = cv::ml::SVM::create();
    ml->setType(SVM::C_SVC);
    ml->setKernel(SVM::LINEAR);
    //ml->setC(312.5);
    //ml->setGamma(1);
    ml->setTermCriteria(cv::TermCriteria(cv::TermCriteria::MAX_ITER,
                                         (int)1e7, 1e-6));

    auto train_data = TrainData::create(features_train_.reshape(1, int(labels_train_.size())),
                                        ROW_SAMPLE,
                                        labels_train_);

    ml->trainAuto(train_data);
    std::cout<<"c : "<<ml->getC()<<std::endl;
    std::cout<<"class weights : "<<ml->getClassWeights()<<std::endl;
    std::cout<<"coef0 : "<<ml->getCoef0()<<std::endl;
    std::cout<<"degree : "<<ml->getDegree()<<std::endl;
    std::cout<<"gamma : "<<ml->getGamma()<<std::endl;
    std::cout<<"nu : "<<ml->getNu()<<std::endl;
    std::cout<<"p : "<<ml->getP()<<std::endl;//*/

    ml->write(cv::FileStorage(result_file_,
                              cv::FileStorage::WRITE));

    {
        //work around for opencv3.0
        cv::FileStorage append(result_file_,
                               cv::FileStorage::APPEND);
        append<<"format"<<3;
    }
    std::cout<<__func__<<" : finish write"<<std::endl;

    ml_ = ml;
}

void train_chars::generate_train_number()
{
    if(!boost::filesystem::exists(chars_folder_)){
        std::cout<<"chars folder do not exist"<<std::endl;
        return;
    }

    auto folders = ocv::file::get_directory_folders(chars_folder_);
    min_symbol_size_ = std::numeric_limits<size_t>::max();
    for(size_t i = 0; i != folders.size(); ++i){
        auto it = bm_.left.find(folders[i]);
        if(it != bm_.left.end()){
            auto const folder = chars_folder_ + "/" + folders[i];
            auto const img_size = ocv::file::get_directory_files(folder).size();
            std::cout<<folder<<" has "<<img_size<<" images"<<std::endl;
            if(img_size < min_symbol_size_){
                min_symbol_size_ = img_size;
            }
        }
    }
    if(min_symbol_size_ == std::numeric_limits<size_t>::max()){
        min_symbol_size_ = 0;
        train_size_ = 0;
        return;
    }
    train_size_ = static_cast<size_t>(min_symbol_size_);
    std::cout<<"min images size is "<<min_symbol_size_<<std::endl;
    std::cout<<"train size is "<<train_size_<<std::endl;
}

void train_chars::show_training_results(const features_type &features,
                                        const label_type &labels)
{
    std::cout<<__func__<<std::endl;
    std::map<std::string, int> statistic;
    generate_map(statistic, mtype_);
    for(auto &pair : statistic){
        pair.second = 0;
    }

    for(size_t i = 0; i != labels.size(); ++i){
        int const label = static_cast<int>(ml_->predict(features[i]));
        if(label == labels[i]){
            auto it = bm_.right.find(label);
            if(it != std::end(bm_.right)){
                statistic[it->second]++;
            }
        }
    }

    double total_accu = 0;
    for(auto const &pair : statistic){
        double const accu = pair.second/static_cast<double>(min_symbol_size_*4);
        std::cout<<pair.first<<" : "<<accu<<std::endl;
        total_accu += accu;
    }
    total_accu /= static_cast<double>(statistic.size());
    std::cout<<"total accuracy : "<<total_accu<<std::endl;
}

cv::Ptr<cv::ml::StatModel> train_chars::train()
{
    describe_features();
    train_classifier();

    return ml_;
}

void train_chars::add_noise(cv::Mat &inout, int label)
{
    std::uniform_int_distribution<size_t> dist(0,10);
    ocv::for_each_channels<uchar>(inout, [&](auto &pix)
    {
        if(dist(generator_) <= 3){
            pix = 255;
        }
    });
    extract_feature(inout, label);
}

void train_chars::add_rotation(cv::Mat &inout, int label,
                               double rotation)
{
    cv::Point2f const center(inout.cols/2.0F, inout.rows/2.0F);
    auto const rmatrix = cv::getRotationMatrix2D(center,
                                                 rotation, 1.0);
    cv::warpAffine(inout, inout, rmatrix, inout.size());
    extract_feature(inout, label);
}
