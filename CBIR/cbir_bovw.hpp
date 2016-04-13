#ifndef CBIR_BOVW_H
#define CBIR_BOVW_H

#include <ocv_libs/cbir/features_indexer.hpp>

#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>

#include <opencv2/core.hpp>

#include <fstream>
#include <string>
#include <vector>

class cbir_bovw
{
public:
    cbir_bovw();

    void run();

private:
    using feature_type = float;

    /**
     * Extract features and keypoints from images
     * and save as hdf5 format
     * @param files
     */
    void add_data();

    /**
     * build the code book
     * @param code_size size of the code book
     */
    void build_code_book(size_t code_size);

    cv::Mat read_img(std::string const &name,
                     bool to_gray = true) const;

    void visualize_code_book(size_t code_size);

    rapidjson::Document setting_;
    ocv::cbir::features_indexer fi_;
    std::ifstream ifs_;
    rapidjson::IStreamWrapper isw_;
};

#endif // CBIR_BOVW_H
