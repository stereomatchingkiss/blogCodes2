#include "train_chars.hpp"
#include "utility.hpp"

#include <ocv_libs/file/utility.hpp>
#include <ocv_libs/ml/utility/split_train_test.hpp>

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <boost/filesystem.hpp>

#include <iostream>
#include <random>

namespace{

template<typename Map>
void generate_map(Map &map)
{
    map.insert({std::to_string(2), 0});
    map.insert({std::to_string(7), 1});
    /*
      int index = 0;
      for(char i = '0'; i <= '9'; ++i){
        map.insert({std::string(1,i), index++});
    }
    /*for(char i = 'a'; i <= 'z'; ++i){
        map.insert({std::string(1,i), index++});
    }//*/
}

}

train_chars::train_chars(std::string chars_folder,
                         std::string result_folder) :
    bbps_({30, 15}, {{5,5}, {5,10}, {10,5}, {10,10}}),
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

    features_train_.resize(0);
    features_validate_.resize(0);
    features_.clear();
    labels_.clear();
    std::random_device rd;
    std::mt19937 g(rd());
    for(size_t i = 0; i != folders.size(); ++i){
        auto const folder = chars_folder_ + "/" + folders[i];
        //std::cout<<folder<<std::endl;

        auto it = bm_labels_int_.left.find(folders[i]);
        if(it != bm_labels_int_.left.end()){
            int const label = it->second;
            std::cout<<__func__<<", label = "<<label<<std::endl;
            auto files = ocv::file::get_directory_files(folder);
            std::shuffle(std::begin(files), std::end(files), g);
            files.resize(train_size_ + validate_size_);

            for(size_t j = 0; j != files.size(); ++j){
                //std::cout<<folder + "/" + files[j]<<std::endl;
                auto img = cv::imread(folder + "/" + files[j]);
                if(!img.empty()){
                    /*if(j == 0){
                        cv::imshow("binarize", binarize_image(img));
                        cv::waitKey();
                        cv::destroyAllWindows();
                    }*/
                    /*std::vector<float> feature;
                    auto bi_img = binarize_image(img);
                    cv::resize(bi_img, bi_img, {30,15});
                    ocv::for_each_channels<uchar>(bi_img,
                                                  [&](auto val)
                    {
                        feature.emplace_back(val);
                    });
                    cv::normalize(feature, feature);//*/
                    auto feature = bbps_.describe(binarize_image(img));
                    //std::cout<<"feautre size == "<<feature.size()<<std::endl;
                    features_.emplace_back(std::move(feature));
                    labels_.emplace_back(label);
                }
            }
        }
    }
}

void train_chars::split_train_test()
{                
    for(size_t i = 0; i != train_size_; ++i){
        features_train_.push_back(cv::Mat(features_[i], true));
        labels_train_.emplace_back(labels_[i]);
    }

    for(size_t i = train_size_; i != train_size_ + validate_size_; ++i){
        features_validate_.push_back(cv::Mat(features_[i], true));
        labels_validate_.emplace_back(labels_[i]);
    }
}

void train_chars::test_train_result()
{    
    std::cout<<"---------train precision---------------"<<std::endl;
    features_type ft;
    std::move(std::begin(features_), std::begin(features_) + train_size_,
              std::back_inserter(ft));
    show_training_results(ft, labels_train_);

    //std::cout<<"---------validate precision---------------"<<std::endl;
    //ft.clear();
    //std::move(std::begin(features_) + train_size_, std::end(features),
    //          std::back_inserter(ft));
    //show_training_results(features_validate_, labels_validate_);
}

void train_chars::describe_features()
{
    extract_features();
    split_train_test();
}

void train_chars::train_classifier()
{
    using namespace cv::ml;

    /*auto ml = cv::ml::RTrees::create();
    ml->setMaxDepth(20);
    ml->setMinSampleCount(2);
    ml->setRegressionAccuracy(0);
    ml->setUseSurrogates(false);
    ml->setMaxCategories(16);
    ml->setPriors(cv::Mat());
    ml->setCalculateVarImportance(false);
    ml->setActiveVarCount(0);
    ml->setTermCriteria({cv::TermCriteria::MAX_ITER,
                         1000, 0});

    auto train_data = TrainData::create(features_train_.reshape(1, int(labels_train_.size())),
                                        ROW_SAMPLE,
                                        labels_train_);

    ml->train(train_data);//*/

    auto ml = cv::ml::SVM::create();
    ml->setType(SVM::C_SVC);
    ml->setKernel(SVM::LINEAR);
    ml->setTermCriteria(cv::TermCriteria(cv::TermCriteria::MAX_ITER,
                                         10000, 1e-6));

    auto train_data = TrainData::create(features_train_.reshape(1, int(labels_train_.size())),
                                        ROW_SAMPLE,
                                        labels_train_);
    ml->trainAuto(train_data);
    std::cout<<"c : "<<ml->getC()<<"\n";
    std::cout<<"coef0 : "<<ml->getCoef0()<<"\n";
    std::cout<<"degree : "<<ml->getDegree()<<"\n";
    std::cout<<"gamma : "<<ml->getGamma()<<"\n";//*/

    //ml->write(cv::FileStorage("chars_classifier.xml",
    //                          cv::FileStorage::WRITE));

    ml_ = ml;
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
        auto it = bm_labels_int_.left.find(folders[i]);
        if(it != bm_labels_int_.left.end()){
            auto const folder = chars_folder_ + "/" + folders[i];
            auto const img_size = ocv::file::get_directory_files(folder).size();
            std::cout<<folder<<" has "<<img_size<<" images"<<std::endl;
            if(img_size < min){
                min = img_size;
            }
        }
    }
    if(min == std::numeric_limits<size_t>::max()){
        min = 0;
    }
    train_size_ = static_cast<size_t>(min);
    validate_size_ = min - train_size_;
    std::cout<<"min images size is "<<min<<std::endl;
}

void train_chars::show_training_results(const features_type &features,
                                        const label_type &labels)
{
    std::map<std::string, int> statistic;
    generate_map(statistic);

    for(size_t i = 0; i != labels.size(); ++i){
        auto label = ml_->predict(features[i]);
        if(label == labels[i]){
            auto it = bm_labels_int_.right.find(label);
            if(it != bm_labels_int_.right.end()){
                statistic[it->second]++;
            }
        }
        //std::cout<<"true label = "<<labels[i]
        //           <<",predict label = "<<label<<std::endl;
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
