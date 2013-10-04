#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <QDir>
#include <QString>
#include <QStringList>

#include "cameraCalibrator.hpp"

//not a big deal to use global parameter in this small program
std::string const Folder("/Users/Qt/program/blogsCodes/pic/images/chessboards/");

QStringList get_images_name(QString const &dir_name)
{
    QDir temp_dir(dir_name);
    if(temp_dir.exists() && !dir_name.isEmpty()){
        auto new_files = temp_dir.entryList(QStringList{"*.jpg"}, QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot);
        for(auto &data : new_files){
            data.prepend(dir_name);
            data = QDir::toNativeSeparators(data);
        }

        return new_files;
    }

    return QStringList();
}

void show_chess_board_corners(std::string const &name)
{
    cv::Mat image = cv::imread(name, CV_LOAD_IMAGE_GRAYSCALE);
    if(image.empty()){
        std::cerr<<"input 1 is empty"<<std::endl;
        return;
    }

    std::vector<cv::Point2f> image_corners;
    cv::Size board_size(6, 4);
    bool found = cv::findChessboardCorners(image, board_size, image_corners);
    cv::drawChessboardCorners(image, board_size, image_corners, found);

    cv::imshow("", image);
    cv::waitKey();
}

void show_chess_boards_corners()
{
    QStringList const names = get_images_name(QString::fromStdString(Folder));
    for(auto const &data : names){
        show_chess_board_corners(data.toStdString());
    }
}

void remap_images()
{
    QStringList const names = get_images_name(QString::fromStdString(Folder));
    std::vector<std::string> filelist;
    //filelist.reserve(names.size());
    /*for(auto const &name : names){
        filelist.emplace_back(name.toStdString());
    }*/
    for(size_t i = 0; i != 20; ++i){
        filelist.emplace_back(names[i].toStdString());
    }

    // Create calibrator object
    cameraCalibrator camCalibrator;
    // add the corners from the chessboard
    cv::Size boardSize(6,4);
    camCalibrator.add_chess_board_points(
                filelist,       // filenames of chessboard image
                boardSize);     // size of chessboard

    // calibrate the camera
    //  cameraCalibrator.setCalibrationFlag(true,true);
    for(size_t i = 0; i != 20; ++i){
        std::cout<<filelist[i]<<std::endl;
        cv::Mat image = cv::imread(filelist[i]);
        camCalibrator.calibrate(image.size());

        // Image Undistortion
        cv::Mat uImage = camCalibrator.remap(image);

        cv::Mat matrix = camCalibrator.get_camera_matrix();
        std::cout<<matrix<<std::endl;

        cv::imshow("original", image);
        cv::imshow("remap", uImage);
        cv::waitKey();
    }
}

int main()
{
    remap_images();
    //show_chess_boards_corners();

    return 0;
}

