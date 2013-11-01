#ifndef CHARACTERRECOGNIZER_HPP
#define CHARACTERRECOGNIZER_HPP

#include <string>

#include <opencv2/core/core.hpp>
#include <opencv2/ml/ml.hpp>

/**
 * @brief train the data and classify the charecter
 */
class characterRecognizer
{
public:
    characterRecognizer() = default;
    characterRecognizer(std::string const &file_name, std::string const &training_name, std::string const &label_name);
    characterRecognizer& operator=(characterRecognizer const&) = delete;
    characterRecognizer(characterRecognizer const&) = delete;

    int classify(cv::Mat const &input);

    void read(std::string const &file_name, std::string const &training_name, std::string const &label_name);
    void read(cv::Mat const &train_data, cv::Mat const &train_labels);

    void train(int nlayers, int num_charecter);
    void train(cv::Mat const &trainData, cv::Mat const &classes, int nlayers, int num_charecter);

private:
    CvANN_MLP ann;
    CvKNearest knn;
    int num_charecter;
    cv::Mat output;

    cv::Mat training_data;
    cv::Mat training_labels;
};

#endif // CHARACTERRECOGNIZER_HPP
