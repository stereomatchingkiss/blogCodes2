#include "../../ocv_libs/core/montage.hpp"

#include <QDebug>
#include <QDir>
#include <QFileInfoList>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <iostream>
#include <vector>

using namespace cv;
using namespace std;

/**
 * This function concat the images in the folder with the same name,
 * the function will find the images under the folders
 * @example
 * folder:
 *   --folder_0:
 *     --img_0.jpg
 *     --img_1.jpg
 *   --folder_1:
 *     --img_0.jpg
 *     --img_1.jpg
 * Will concat images as
 * img_0 + img_0 = concat_img_0.jpg
 * img_1 + img_1 = concat_img_1.jpg
 * @param fs
 * @warning 1. Images in each folder must have the same name, same images quantity, suffix
 * could be different
 * 2. Every image with the same name must have the same size
 */
void concat_images_in_folders(FileStorage const &fs)
{        
    if(fs["mode"].string() == "concat_images_in_folders"){
        QString const folder(fs["folder"].string().c_str());
        auto const folder_list = QDir(folder).entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
        if(!folder_list.isEmpty()){
            QDir().mkpath(fs["save_at"].string().c_str());
            auto const img_list =
                    QDir(folder_list[0].absoluteFilePath()).entryInfoList(QDir::Files | QDir::NoDotAndDotDot);
            for(int i = 0; i != img_list.size(); ++i){
                qDebug()<<"processing image:"<<img_list[i].fileName();
                std::vector<cv::Mat> images;
                for(int j = 0; j != folder_list.size(); ++j){
                    auto const img_path = folder_list[j].absoluteFilePath() + "/" + img_list[i].fileName();
                    auto img = cv::imread(img_path.toStdString());
                    if(!img.empty()){
                        images.emplace_back(std::move(img));
                    }
                }

                if(!images.empty()){
                    ocv::montage mt({images[0].cols,images[0].rows}, folder_list.size(), 1);
                    for(auto const &img : images){
                        mt.add_image(img);
                    }
                    auto const mimg = mt.get_montage();
                    cv::imwrite(fs["save_at"].string() + "/" + img_list[i].fileName().toStdString(), mimg);
                }
            }
        }else{
            cerr<<"folder list is empty"<<endl;
        }
    }
}

int main(int argc, char *argv[])
{
    if(argc < 2){
        cerr<<"Please specify the location of the json"<<endl;
        return -1;
    }

    FileStorage fs;
    fs.open(argv[1], cv::FileStorage::READ);
    if(fs.isOpened()){
        concat_images_in_folders(fs);
    }else{
        cerr<<"cannot open file"<<endl;
    }

    return 0;
}
