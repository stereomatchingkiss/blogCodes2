#include "train_chars.hpp"
#include "utility.hpp"

#include <ocv_libs/file/utility.hpp>

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <boost/filesystem.hpp>

#include <iostream>
#include <random>

namespace{

template<typename Map>
void generate_map(Map &map)
{
    //map.insert({std::to_string(1), 0});
    //map.insert({std::to_string(2), 1});

    int index = 0;
    for(char i = '0'; i <= '9'; ++i){
        map.insert({std::string(1,i), index++});
    }
    for(char i = 'a'; i <= 'z'; ++i){
        map.insert({std::string(1,i), index++});
    }//*/
}

}

train_chars::train_chars(std::string chars_folder,
                         std::string result_folder) :    
    chars_folder_(std::move(chars_folder)),
    result_folder_(std::move(result_folder))
{
    using namespace boost::filesystem;

    if(!exists(path(result_folder_))){
        create_directory(path(result_folder_));
    }

    generate_map(bm_labels_int_);
    generate_train_number();
}

void train_chars::extract_features()
{
    auto const folders =
            ocv::file::get_directory_folders(chars_folder_);

    features_.clear();
    features_train_.resize(0);
    labels_.clear();
    std::random_device rd;
    std::mt19937 g(rd());
    for(size_t i = 0; i != folders.size(); ++i){
        auto const folder = chars_folder_ + "/" + folders[i];
        auto it = bm_labels_int_.left.find(folders[i]);
        if(it != std::end(bm_labels_int_.left)){
            int const label = it->second;
            std::cout<<__func__<<", label = "<<label<<std::endl;
            auto files = ocv::file::get_directory_files(folder);
            std::shuffle(std::begin(files), std::end(files), g);

            for(size_t j = 0; j != min_symbol_size_; ++j){
                auto img = cv::imread(folder + "/" + files[j]);
                if(!img.empty()){
                    auto feature = bbps_.describe(binarize_image(img));
                    features_.emplace_back(std::move(feature));
                    labels_.emplace_back(label);
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

    auto ml = cv::ml::RTrees::create();
    ml->setMaxDepth(30);
    ml->setMinSampleCount(static_cast<int>(labels_train_.size() * 0.01));
    ml->setRegressionAccuracy(0);
    ml->setUseSurrogates(false);
    ml->setMaxCategories(16);
    ml->setPriors(cv::Mat());
    ml->setCalculateVarImportance(false);
    ml->setActiveVarCount(0);
    ml->setTermCriteria({cv::TermCriteria::MAX_ITER,
                         10000, 1e-6});

    auto train_data = TrainData::create(features_train_.reshape(1, int(labels_train_.size())),
                                        ROW_SAMPLE,
                                        labels_train_);

    ml->train(train_data);//*/

    /*auto ml = cv::ml::SVM::create();
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
    std::cout<<"c : "<<ml->getC()<<"\n";
    std::cout<<"coef0 : "<<ml->getCoef0()<<"\n";
    std::cout<<"degree : "<<ml->getDegree()<<"\n";
    std::cout<<"gamma : "<<ml->getGamma()<<"\n";//*/

    ml->write(cv::FileStorage(result_folder_ + "/chars_classifier.xml",
                              cv::FileStorage::WRITE));

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
    size_t exist_symbol = 0;
    for(size_t i = 0; i != folders.size(); ++i){
        auto it = bm_labels_int_.left.find(folders[i]);
        if(it != bm_labels_int_.left.end()){
            ++exist_symbol;
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
    std::map<std::string, int> statistic;
    generate_map(statistic);
    for(auto &pair : statistic){
        pair.second = 0;
    }

    std::cout<<"labels size : "<<labels.size()<<std::endl;
    std::vector<int> labels_count(10, 0);
    for(size_t i = 0; i != labels.size(); ++i){
        ++labels_count[labels[i]];
        int const label = static_cast<int>(ml_->predict(features[i]));
        if(label == labels[i]){
            auto it = bm_labels_int_.right.find(label);
            if(it != std::end(bm_labels_int_.right)){
                statistic[it->second]++;
            }
        }
    }
    for(size_t i = 0; i != labels_count.size(); ++i){
        std::cout<<i<<" has "<<labels_count[i]<<std::endl;
    }
    for(auto const &pair : statistic){
        std::cout<<pair.first<<" : "<<pair.second<<std::endl;
    }
}

cv::Ptr<cv::ml::StatModel> train_chars::train()
{
    describe_features();
    train_classifier();

    return ml_;
}
