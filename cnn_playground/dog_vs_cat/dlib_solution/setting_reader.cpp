#include "setting_reader.hpp"

#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>

#include <fstream>
#include <iostream>

using namespace rapidjson;

struct setting_reader::impl
{
    impl();
    impl(std::string const &setup_file);

    void read_setup_file(std::string const &setup_file);
    std::vector<std::string> read_vector_str(std::string const &target_folder, std::string const &target_files)
    {
        std::vector<std::string> results;
        Value &files = document_[target_files.c_str()];
        std::string const folder = document_[target_folder.c_str()].GetString();
        for (SizeType i = 0; i < files.Size(); ++i){
            results.emplace_back(folder + "/" + files[i].GetString());
        }

        return results;
    }

    Document document_;
};

setting_reader::setting_reader() :
    pimpl_(new impl)
{

}

setting_reader::setting_reader(std::string const &setup_file) :
    pimpl_(new impl(setup_file))
{

}

setting_reader::~setting_reader()
{

}

bool setting_reader::augment_enable() const
{
    return pimpl_->document_["augment_enable"].GetBool();
}

size_t setting_reader::augment_ratio() const
{
    return pimpl_->document_["augment_ratio"].GetInt();
}

size_t setting_reader::batch_size() const
{
    return pimpl_->document_["batch_size"].GetInt();
}

std::string setting_reader::dlib_resnet34_model() const
{
    return pimpl_->document_["dlib_resnet34_model"].GetString();
}

size_t setting_reader::kcross() const
{
    return pimpl_->document_["kcross"].GetInt();
}

std::vector<size_t> setting_reader::kcross_file() const
{
    std::vector<size_t> results;
    Value &files = pimpl_->document_["kcross_file"];
    for (SizeType i = 0; i < files.Size(); ++i){
        results.emplace_back(files[i].GetInt());
    }

    return results;
}

double setting_reader::min_learning_rate() const
{
    return pimpl_->document_["min_learning_rate"].GetDouble();
}

double setting_reader::max_learning_rate() const
{
    return pimpl_->document_["max_learning_rate"].GetDouble();
}

std::string setting_reader::submit_file() const
{
    return pimpl_->document_["submit_file"].GetString();
}

std::string setting_reader::test_features() const
{
    return std::string(pimpl_->document_["store_test_features"].GetString()) +
            "/" + pimpl_->document_["test_features"].GetString();
}

std::string setting_reader::test_labels() const
{
    return std::string(pimpl_->document_["store_test_features"].GetString()) +
            "/" + pimpl_->document_["test_labels"].GetString();
}

void setting_reader::read_setup_file(const std::string &setup_file)
{
    pimpl_->read_setup_file(setup_file);
}

std::string setting_reader::store_test_features() const
{
    return pimpl_->document_["store_test_features"].GetString();
}

std::string setting_reader::store_train_features() const
{
    return pimpl_->document_["store_train_features"].GetString();
}

std::string setting_reader::test_files_folder() const
{
    return pimpl_->document_["test_files_folder"].GetString();
}

std::string setting_reader::train_cat_folder() const
{
    return pimpl_->document_["train_cat_folder"].GetString();
}

std::string setting_reader::train_dog_folder() const
{
    return pimpl_->document_["train_dog_folder"].GetString();
}

std::vector<std::string> setting_reader::trained_features() const
{
    return pimpl_->read_vector_str("store_train_features", "train_features");
}

std::vector<std::string> setting_reader::trained_labels() const
{
    return pimpl_->read_vector_str("store_train_features", "train_labels");
}

std::vector<std::string> setting_reader::trained_models() const
{
    return pimpl_->read_vector_str("trained_models_folder", "trained_models");
}

std::string setting_reader::trained_models_folder() const
{
    return pimpl_->document_["trained_models_folder"].GetString();
}

bool setting_reader::train_with_pseudo_layer() const
{
    return pimpl_->document_["train_with_pseudo_layer"].GetBool();
}

setting_reader::impl::impl()
{

}

setting_reader::impl::impl(std::string const &setup_file)
{
    read_setup_file(setup_file);
}

void setting_reader::impl::read_setup_file(const std::string &setup_file)
{
    std::ifstream ifs(setup_file);
    if(!ifs.is_open()){
        throw std::runtime_error("cannot open file:" + setup_file);
    }else{
        std::cout<<"can open:"<<setup_file<<std::endl;
    }
    IStreamWrapper isw(ifs);
    document_.ParseStream(isw);
}
