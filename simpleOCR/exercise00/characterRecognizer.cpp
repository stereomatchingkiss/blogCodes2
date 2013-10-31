#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/ml/ml.hpp>

#include "characterRecognizer.hpp"

characterRecognizer::characterRecognizer(std::string const &file_name, std::string const &training_name, std::string const &label_name)
    : num_charecter(0)
{
    if(!file_name.empty()){
        read(file_name, training_name, label_name);
    }
}

/**
 * @brief classify the characters of input by CvANN_MLP
 * @param input : input image
 * @return the associated label
 */
int characterRecognizer::classify(cv::Mat const &input)
{
    output.create(1, num_charecter, CV_32F);
    ann.predict(input, output);
    cv::Point maxLoc;
    cv::minMaxLoc(output, nullptr, nullptr, nullptr, &maxLoc);
    //We need know where in output is the max val, the x (cols) is the class.

    return maxLoc.x;
}

/**
 * @brief set up the training data and training labels from file
 * @param file_name : name of the files(with full path)
 */
void characterRecognizer::read(std::string const &file_name, std::string const &training_name, std::string const &label_name)
{
    cv::FileStorage fs(file_name, cv::FileStorage::READ);
    fs[training_name] >> training_data;
    fs[label_name] >> training_labels;
}

/**
 * @brief set up the training data and training labels
 * @param train_data : the training data
 * @param train_labels : the training labels
 */
void characterRecognizer::read(cv::Mat const &train_data, cv::Mat const &train_labels)
{
    training_data = train_data;
    training_labels = train_labels;
}

/**
 * @brief train the samples by neural network, make sure you have setup
 * training data and training labels before you call this function
 * @param nlayers : layers of the network
 * @param num_charecter : number of the characters,
 * it is equal to the type of the training labels(training classes)
 */
void characterRecognizer::train(int nlayers, int num_charecter)
{
    train(training_data, training_labels, nlayers, num_charecter);
}

/**
 * @brief overload of train, you can specity the training data and training labels by
 * this overload function
 * @param train_data : the training data
 * @param labels : the training labels
 * @param nlayers : layers of the network
 * @param num_char : number of the characters,
 * it is equal to the type of the training labels(training classes)
 */
void characterRecognizer::train(cv::Mat const &train_data, cv::Mat const &labels, int nlayers, int num_char)
{
    CV_Assert(!train_data.empty() && !labels.empty());

    num_charecter = num_char;
    int buffer[] = {train_data.cols, nlayers, num_char};
    cv::Mat const layers(1, 3, CV_32S, buffer);
    ann.create(layers, CvANN_MLP::SIGMOID_SYM, 1, 1);

    //Prepare trainClases
    //Create a mat with n trained data by m classes
    cv:: Mat train_classes;
    train_classes.create(train_data.rows, num_char, CV_32F);
    for( int i = 0; i !=  train_classes.rows; ++i){
        int const label = *labels.ptr<int>(i);
        auto train_ptr = train_classes.ptr<float>(i);
        for(int k = 0; k != train_classes.cols; ++k){
            *train_ptr = k != label ? 0 : 1;
            ++train_ptr;
        }
    }

    cv::Mat const weights = cv::Mat::ones(1, train_data.rows, CV_32FC1);

    //Learn classifier
    ann.train(train_data, train_classes, weights);
}
