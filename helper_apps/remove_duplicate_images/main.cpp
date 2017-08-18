#include <vp_tree.hpp>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/img_hash.hpp>
#include <opencv2/core/ocl.hpp>

#include <boost/filesystem.hpp>

#include <dlib/threads.h>

#include <iostream>
#include <set>

using namespace std;
using namespace boost::filesystem;

struct dist_compare
{
    using value_type = std::pair<path, cv::Mat>;
    using algo_type = cv::Ptr<cv::img_hash::ImgHashBase>;

    explicit dist_compare(algo_type algo) :
        algo_(algo){}

    double operator()(value_type const &lhs,
                      value_type const &rhs) const
    {
        return algo_->compare(lhs.second, rhs.second);
    }

private:
    algo_type algo_;
};

int main(int argc, char *argv[])
{
    if(argc != 3){
        std::cerr<<"enter folder want to remove duplicate images, and the threshold of PHash.\n"
                   "Example : ./remove_duplicate_images /home/tlk/faces 5";
        return -1;
    }

    cv::ocl::setUseOpenCL(false);

    std::set<std::string> const valid_suffix{".jpg", ".jpeg", ".png", ".bmp"};
    std::vector<path> images_path;
    for(recursive_directory_iterator iter(argv[1]); iter != recursive_directory_iterator(); ++iter){
        path p(*iter);
        if(valid_suffix.find(p.extension().string()) != valid_suffix.end()){
            images_path.emplace_back(p);
        }
    }

    auto phash = cv::img_hash::PHash::create();
    std::vector<dist_compare::value_type> hash_arr;
    for(size_t i = 0; i != images_path.size(); ++i){
        if(i % 100 == 0){
            std::cout<<"compute hash of:"<<images_path[i]<<std::endl;
        }
        cv::Mat const img = cv::imread(images_path[i].string());
        if(!img.empty()){
            cv::Mat hash;
            phash->compute(img, hash);
            hash_arr.emplace_back(std::move(images_path[i]), hash);
        }
    }

    double const threshold = std::stod(argv[2]);
    dist_compare dc(std::move(phash));
    vp_tree<dist_compare::value_type, dist_compare> tree(std::move(dc));
    tree.create(std::move(hash_arr));
    
    //Single thread version of 
    /*std::vector<dist_compare::value_type> closest_hash;
    std::vector<double> distance;
    size_t removed_file = 0;
    for(size_t i = 0; i < images_path.size(); ++i){
        if(i % 100 == 0){
            std::cout<<"removed_file:"<<removed_file<<",compare :"<<images_path[i]<<std::endl;
        }
        if(exists(images_path[i])){
            tree.search(tree.get_items()[i], 10, closest_hash, distance,
                        [=](double val){ return val <= threshold; });
            if(i % 100 == 0){
                std::cout<<"closest_hash:"<<closest_hash.size()<<std::endl;
            }
            if(closest_hash.size() > 1){
                for(size_t j = 0; j != closest_hash.size(); ++j){
                    if(exists(closest_hash[j].first) && tree.get_items()[i].first != closest_hash[j].first){
                        boost::filesystem::remove(closest_hash[j].first);
                        ++removed_file;
                    }
                }
            }
        }
    }//*/

    size_t removed_file(0);
    std::mutex mutex;
    dlib::parallel_for(0, static_cast<long>(images_path.size()), [&](long i)
    {
        if(i % 100 == 0){
            std::cout<<"remove similar file of:"<<images_path[i]<<"\n";
        }
        if(exists(images_path[i])){
            std::vector<dist_compare::value_type> closest_hash;
            std::vector<double> distance;
            tree.search(tree.get_items()[i], 10, closest_hash, distance,
                        [=](double val){ return val <= threshold; });
            if(closest_hash.size() > 1){
                for(size_t j = 0; j != closest_hash.size(); ++j){
                    if(tree.get_items()[i].first != closest_hash[j].first){
                        std::lock_guard<std::mutex> lock(mutex);
                        if(boost::filesystem::remove(closest_hash[j].first)){
                            ++removed_file;
                        }
                    }
                }
            }
        }
    });

    std::cout<<"removed_file:"<<removed_file<<std::endl;

    return 0;
}
