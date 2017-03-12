#ifndef SETTING_READER_HPP
#define SETTING_READER_HPP

#include <memory>
#include <string>
#include <vector>

class setting_reader
{
public:
    setting_reader();
    explicit setting_reader(std::string const &setup_file);
    ~setting_reader();

    bool augment_enable() const;
    size_t augment_ratio() const;
    size_t batch_size() const;
    std::string dlib_resnet34_model() const;
    size_t kcross() const;
    std::vector<size_t> kcross_file() const;
    double max_learning_rate() const;
    double min_learning_rate() const;        
    void read_setup_file(std::string const &setup_file);
    std::string store_test_features() const;
    std::string store_train_features() const;
    std::string submit_file() const;
    std::string test_features() const;
    std::string test_labels() const;
    std::string test_files_folder() const;
    std::string train_cat_folder() const;
    std::string train_dog_folder() const;
    std::vector<std::string> trained_features() const;
    std::vector<std::string> trained_labels() const;
    std::vector<std::string> trained_models() const;
    std::string trained_models_folder() const;
    bool train_with_pseudo_layer() const;

private:
    struct impl;
    std::unique_ptr<impl> pimpl_;
};

#endif // SETTING_READER_HPP
