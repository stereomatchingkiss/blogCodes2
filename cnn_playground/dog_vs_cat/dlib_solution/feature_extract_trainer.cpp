#include "feature_extract_trainer.hpp"
#include "../../general_functions/dlib/feature_extractor_resnet34.hpp"

#include "print_result.hpp"
#include "read_input_data.hpp"
#include "setting_reader.hpp"
#include "transfer_net.hpp"

#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>

#include <ocv_libs/ml/utility/split_train_test.hpp>

#include <dlib/data_io.h>
#include <dlib/dir_nav.h>

#include <thread>

#include <chrono>
#include <iostream>

using namespace dlib;
using namespace std;

namespace{

using net_type = transfer::net_08;

using fimg_type = matrix<rgb_pixel>;
using img_type = matrix<float>;
using label_type = unsigned long;

setting_reader setup;

std::tuple<std::vector<img_type> , std::vector<label_type>, std::vector<std::string>>
extract_features(std::vector<image_info> const &input,
                 feature_extractor_resnet34 &pretrain_net,
                 bool augment)
{
    std::vector<img_type> results;
    std::vector<label_type> labels;
    std::vector<std::string> names;
    for(auto const &info : input){
        cout<<"extract features of "<<info.filename_<<endl;
        auto tuples = read_img<fimg_type, label_type>(info, augment, 227, 227,
                                                      setup.augment_ratio());
        auto features = pretrain_net.get_features(get<0>(tuples));
        std::move(std::begin(features), std::end(features), std::back_inserter(results));
        labels.insert(std::end(labels), std::begin(std::get<1>(tuples)),
                      std::end(std::get<1>(tuples)));
        for(size_t i = 0; i != std::get<0>(tuples).size(); ++i){
            names.emplace_back(info.filename_);
        }
    }

    return std::make_tuple(std::move(results), std::move(labels), std::move(names));
}

/**
 * @brief split images in the folder to kset
 * @param kset Number of K
 * @example If K is 2, will generate two files.
 * k0.txt, k1.txt
 */
void split_to_kset_data(size_t kset)
{    
    std::vector<image_info> cat_info = read_img_info(setup.train_cat_folder());
    std::vector<image_info> dog_info = read_img_info(setup.train_dog_folder());
    std::vector<image_info> img_info(std::move(cat_info));
    std::move(std::begin(dog_info), std::end(dog_info), std::back_inserter(img_info));
    std::vector<std::string> names;
    std::vector<label_type> labels;
    for(auto const &info : img_info){
        names.emplace_back(info.filename_);
        labels.emplace_back(info.numeric_label_);
    }
    auto data_sets = ocv::ml::split_to_kset_balance(names, labels, kset);
    for(auto const &pair : data_sets){
        cout<<"label : "<<pair.first<<endl;
        auto const &data = pair.second;
        for(size_t i = 0; i != data.size(); ++i){
            std::cout<<"save as:"<<"k" + std::to_string(i) + ".txt"<<std::endl;
            ofstream out("k" + std::to_string(i) + ".txt",
                         ofstream::out | ofstream::app);
            for(auto const &elem : data[i]){
                out<<pair.first<<"\t"<<elem<<"\n";
            }
        }
    }
}

void store_test_feature()
{    
    dlib::directory dir(setup.test_files_folder());
    auto const files = dir.get_files();
    std::vector<img_type> img_features;
    std::vector<size_t> img_labels;
    std::vector<std::string> img_names;
    feature_extractor_resnet34 feature_extractor(setup.dlib_resnet34_model());
    for(auto const &file : files){
        auto const imgs = read_img<fimg_type>(file.full_name(), false, 227, 227, 0);
        std::cout<<"extract features of : "<<file.name()<<std::endl;
        size_t const id = std::stoi(file.name().substr(0, file.name().find_first_of(".")));
        auto features = feature_extractor.get_features(imgs);
        std::move(std::begin(features), std::end(features), std::back_inserter(img_features));
        for(size_t i = 0; i != imgs.size(); ++i){
            img_labels.emplace_back(id);
            img_names.emplace_back(file.full_name());
        }
    }
    serialize("test_data.txt")<<img_features;
    serialize("test_label.txt")<<img_labels;
    serialize("test_name.txt")<<img_names;
}

void store_training_feature()
{
    feature_extractor_resnet34 feature_extractor(setup.dlib_resnet34_model());
    std::string const augment_ratio = std::to_string(setup.augment_ratio());
    std::string const prefix = "k_" + std::to_string(setup.kcross() - 1) + "_" + augment_ratio;
    for(size_t i = 0; i < setup.kcross(); ++i){
        std::cout<<(setup.store_train_features() + "/k" +
                    std::to_string(i) + ".txt")<<endl;
        std::vector<image_info> training_info =
                read_image_info_from_file(setup.store_train_features() + "/k" +
                                          std::to_string(i) + ".txt");
        cout<<"training info size : "<<training_info.size()<<endl;
        std::vector<img_type> training_features;
        std::vector<label_type> training_labels;
        std::vector<std::string> training_names;
        std::tie(training_features, training_labels, training_names) =
                extract_features(training_info, feature_extractor, setup.augment_enable());
        cout<<"final training features size : "<<training_features.size()<<endl;
        cout<<"final training labels size : "<<training_labels.size()<<endl;
        serialize(prefix + "augment_data.txt")<<training_features;
        serialize(prefix + "augment_label.txt")<<training_features;
        serialize(prefix + "augment_names.txt")<<training_features;
    }
}

void push_back_extract_features(std::string const &feature_file,
                                std::string const &label_file,
                                std::vector<img_type> &features,
                                std::vector<label_type> &labels)
{
    std::vector<img_type> tfeatures;
    std::vector<label_type> tlabels;
    deserialize(feature_file)>>tfeatures;
    deserialize(label_file)>>tlabels;
    std::move(std::begin(tfeatures), std::end(tfeatures), std::back_inserter(features));
    std::move(std::begin(tlabels), std::end(tlabels), std::back_inserter(labels));
}

size_t miss_k_file(std::vector<size_t> const &k_cross_index)
{
    size_t const kcross = setup.kcross();
    std::set<size_t> sets(std::begin(k_cross_index), std::end(k_cross_index));
    for(size_t i = 0; i != kcross; ++i){
        if(sets.find(i) == std::end(sets)){
            return i;
        }
    }

    throw std::runtime_error("cannot find missing k file");
}

template<typename Net>
std::string train_net(Net &net, std::vector<img_type> const &train_imgs,
                      std::vector<label_type> const &train_labels,
                      size_t miss_k)
{
    std::cout<<"train net"<<std::endl;
    dnn_trainer<Net, adam> trainer(net);
    std::cout<<"train init end"<<std::endl;
    trainer.set_learning_rate(setup.max_learning_rate());
    trainer.set_min_learning_rate(setup.min_learning_rate());
    trainer.set_mini_batch_size(setup.batch_size());
    trainer.be_verbose();
    //trainer.set_synchronization_file(setup.submit_file() + "_sync", std::chrono::minutes(5));
    std::cout<<"start train"<<endl;
    trainer.train(train_imgs, train_labels);
    std::cout<<"end train"<<endl;

    net.clean();
    std::string const net_name = setup.submit_file() + "_" + std::to_string(setup.kcross()) + "_" +
            std::to_string(miss_k) + "_network.dat";
    serialize(net_name) << net;

    return net_name;
}

void add_pseudo_features(std::vector<size_t> const &k_cross_index,
                         std::vector<img_type> &training_features,
                         std::vector<label_type> &training_labels)
{
    //wrong implementation, pseudo labels should mix with real labels with
    //some sorts of ratio, ex : 2/3 true labels, 1/3 pseudo labels
    if(setup.train_with_pseudo_layer()){
        auto const trainded_models_location = setup.trained_models();
        std::vector<img_type> test_features;
        deserialize(setup.test_features())>>test_features;
        std::vector<label_type> pseudo_labels(test_features.size(), 0);
        for(auto k : k_cross_index){
            std::cout<<"predict pseudo label by model:"<<trainded_models_location[k]<<std::endl;
            net_type net;
            deserialize(trainded_models_location[k])>>net;
            softmax<net_type::subnet_type> snet;
            snet.subnet() = net.subnet();
            for(size_t i = 0; i != pseudo_labels.size(); ++i){
                pseudo_labels[i] += dlib::mat(snet(test_features[i]))(1);
            }
        }
        std::move(std::begin(test_features), std::end(test_features), std::back_inserter(training_features));
        for(auto label : pseudo_labels){
            auto const result = label/k_cross_index.size() > 0.5 ? global::dog_label : global::cat_label;
            training_labels.emplace_back(result);
        }        
    }
}

std::string train(std::vector<size_t> const &k_cross_index)
{
    std::vector<img_type> training_features;
    std::vector<label_type> training_labels;
    auto const trained_labels_location = setup.trained_labels();
    auto const trained_features_location = setup.trained_features();
    for(size_t i = 0; i != k_cross_index.size(); ++i){
        std::cout<<trained_labels_location[i]<<std::endl;
        std::cout<<trained_features_location[i]<<std::endl;
        push_back_extract_features(trained_features_location[k_cross_index[i]],
                trained_labels_location[k_cross_index[i]],
                training_features,
                training_labels);
        cout<<training_features.size()<<","<<training_labels.size()<<endl;
    }

    add_pseudo_features(k_cross_index, training_features, training_labels);

    ocv::ml::shuffles(training_features, training_labels);

    net_type net;
    auto const start = std::chrono::system_clock::now();
    size_t const miss_k = miss_k_file(k_cross_index);
    auto const trained_net_name = train_net(net, training_features, training_labels, miss_k);
    auto const end = std::chrono::system_clock::now();

    std::chrono::duration<double> const elapsed_seconds = end-start;
    std::time_t const end_time = std::chrono::system_clock::to_time_t(end);
    std::cout << "finished training at " << std::ctime(&end_time)
              << "elapsed time: " << elapsed_seconds.count() << "s\n";

    print_train_result(training_features, training_labels, net(training_features), "training");
    training_features.clear();
    training_labels.clear();

    std::vector<img_type> validate_features;
    std::vector<label_type> validate_labels;
    auto const features_dir = setup.trained_features();
    auto const labels_dir = setup.trained_labels();
    push_back_extract_features(features_dir[miss_k],
                               labels_dir[miss_k],
                               validate_features,
                               validate_labels);
    print_train_result(validate_features, validate_labels, net(validate_features), "validate");
    std::cout<<"k cross : "<<setup.kcross()<<std::endl;

    return trained_net_name;
}

void test(std::vector<int> const &k_cross, std::vector<std::string> const &trained_net)
{
    std::vector<img_type> test_imgs;
    std::vector<size_t> id;
    deserialize(setup.test_features())>>test_imgs;
    deserialize(setup.test_labels())>>id;

    net_type net;
    std::map<size_t, float> score;
    for(auto const k : k_cross){
        deserialize(trained_net[k])>>net;
        softmax<net_type::subnet_type> snet;
        snet.subnet() = net.subnet();

        for(size_t i = 0; i != id.size(); ++i){
            score[id[i]] += dlib::mat(snet(test_imgs[i]))(1);
        }
    }

    std::ofstream out(setup.submit_file() + ".txt");
    out<<"id,label"<<endl;

    for(auto const &pair : score){
        out<<pair.first<<","<<pair.second / k_cross.size()<<endl;
    }    
}

} //nameless namespace

void feature_extract_trainer(std::string const &setup_file)
{
    try{
        setup.read_setup_file(setup_file);        
        //split_to_kset_data(setup.kcross());
        //store_test_feature();
        //store_training_feature(global::kaggle_train_5_fold_folder, 5);

        std::vector<std::vector<size_t>> const kcross_files{
            {1,2,3,4},{0,2,3,4},{0,1,3,4},{0,1,2,4},{0,1,2,3}
        };
        std::vector<std::string> trained_net;
        for(auto const &vec : kcross_files){
            trained_net.emplace_back(train(vec));
        }
        test({0,1,2,3,4}, trained_net);
    }catch(std::exception const &ex){
        std::cout<<ex.what()<<std::endl;
    }
}
