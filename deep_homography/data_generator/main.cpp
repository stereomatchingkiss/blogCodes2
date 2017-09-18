/**
 * This program generate training and test data mention by the paper--
 * "Deep Image Homography Estimation(https://arxiv.org/pdf/1606.03798.pdf)"
 */

#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QFileInfo>

#include <opencv2/calib3d.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <fstream>
#include <random>

void create_parser(QCommandLineParser &parser);

/**
 * generate_image_pair generate a pair of images
 * mentioned by section 3 of the paper and a file
 * called info.txt, this file record the info of
 * generated image with format
 *
 * name of patch I\tname of patch I'\tdelta x1\tdelta y1\t
 * delta x2\tdelta y2\tdelta x3\tdelta y3\tdelta x4\tdelta y4\n
 *
 * (x1,y1)~(x4,y4) are top left, top right, bottom left, bottom right
 * Too many parameters, since this is a small, easy
 * to maintain program, please spare with it.
 *
 * @param img_name name of the image
 * @param save_at where to save the generated images
 * @param save_as_color true, save image as color image, false save as gray image
 * @param out_file this file will save the corners points of associate image
 * @param dist uniform int distribution to pretube the points of input
 * @param rd random number generator of dist
 * @param debug true will print/show debug message/image and vice versa
 * @return true if image pair can be generated and vice versa
 */
bool generate_image_pair(QString const &img_name,
                         QString const &save_at,
                         bool save_as_color,
                         std::ofstream &out_file,
                         std::uniform_int_distribution<int> &dist,
                         std::random_device &rd,
                         bool debug);

int main(int argc, char *argv[])try
{
    QCoreApplication a(argc, argv);

    QCommandLineParser parser;
    create_parser(parser);
    parser.process(a);

    if(!parser.isSet("output")){
        qDebug()<<"output is empty";
        return -1;
    }else{
        QDir dir;
        dir.mkdir(parser.value("output"));
    }

    if(!parser.isSet("folder")){
        qDebug()<<"folder is empty";
        return -1;
    }

    qDebug()<<"folder is set:"<<parser.value("folder");

    QDirIterator dir(parser.value("folder"),
                     QStringList()<<"*.jpg"<<"*.png"<<"*.bmp"<<"*.jpeg",
                     QDir::NoDotAndDotDot | QDir::Files, QDirIterator::Subdirectories);
    size_t i = 0;
    constexpr int pertube = 32;
    std::random_device rd;
    std::uniform_int_distribution<int> dist(-pertube, pertube);
    size_t const max_size = parser.value("max_size").toInt();
    size_t gen_image_size = 0;
    std::ofstream out_file((parser.value("output") + "/info.txt").toStdString());
    bool const save_as_color = parser.value("debug").compare("false", Qt::CaseInsensitive) == 0 ? false : true;
    while(dir.hasNext()){
        bool const debug = parser.value("debug").compare("false", Qt::CaseInsensitive) == 0 ? false : true;
        if(debug && i == 10){
            break;
        }
        QFileInfo const info(dir.next());
        qDebug()<<i++<<":"<<info.fileName();
        bool const can_gen_image = generate_image_pair(info.absoluteFilePath(),
                                                       parser.value("output"),
                                                       save_as_color, out_file,
                                                       dist, rd, debug);
        if(can_gen_image){
            ++gen_image_size;
            if(gen_image_size >= max_size){
                break;
            }
        }
    }

    return 0;
}catch(std::exception const &ex){
    qDebug()<<ex.what();
}

using point_type = std::array<cv::Point2f, 4>;

/**
 * Pertube the points
 * @param input Points sort by order : top left, top right,
 * bottom left, bottom right
 * @param dist uniform int distribution to pretube the points of input
 * @param rd random number generator of dist
 * @return Points after pertubed, sort by order : top left, top right,
 * bottom left, bottom right
 */
point_type pertube_points(point_type const &input,
                          std::uniform_int_distribution<int> &dist, std::random_device &rd)
{
    point_type results;
    for(size_t i = 0; i != results.size(); ++i){
        results[i] = cv::Point2f(input[i].x + dist(rd), input[i].y + dist(rd));
    }

    return results;
}

/**
 * Random crop a rectangle
 * @param input_size size of the input image
 * @param crop_size length and width of the crop rects
 * @param dist uniform int distribution to pretube the points of input
 * @param rd random number generator of dist
 * @return Corners points sort by order : top left, top right,
 * bottom left, bottom right
 */
point_type random_crop(cv::Size const &input_size, int crop_size,
                       std::uniform_int_distribution<int> &dist, std::random_device &rd)
{
    point_type results;
    int const center_x = input_size.width / 2 + dist(rd);
    int const center_y = input_size.height / 2 + dist(rd);
    crop_size = crop_size / 2;
    results[0] = cv::Point2f(center_x - crop_size, center_y - crop_size); //top left
    results[1] = cv::Point2f(center_x + crop_size, center_y - crop_size); //top right
    results[2] = cv::Point2f(center_x - crop_size, center_y + crop_size); //bottom left
    results[3] = cv::Point2f(center_x + crop_size, center_y + crop_size); //bottom right

    return results;
}

void create_parser(QCommandLineParser &parser)
{
    parser.setApplicationDescription("Generate homography data for training/testing deep homography");
    parser.addHelpOption();

    parser.addOption({{"f", "folder"},
                      "Folder of the images, this program will search the images("
                      "png, jpg, jpeg, bmp) recursively.",
                      "folder"});
    parser.addOption({{"m", "max_size"},
                      "Maximum size of generated data.Default value is 10000",
                      "max_size", "10000"});
    parser.addOption({{"o", "output"},
                      "Where to save generated images and homography",
                      "output"});
    parser.addOption({{"d", "debug"},
                      "true will print/show debug message/image, false will not. Default value is false",
                      "debug", "false"});
    parser.addOption({{"c", "color"},
                      "true will save generated image as color image(b,g,r), false will be gray image.\n"
                      "Default value is false",
                      "color", "false"});
}

void draw_lines(point_type const &points, cv::Mat &inout)
{
    cv::line(inout, points[0], points[1], {255, 0, 0}, 2);
    cv::line(inout, points[0], points[2], {0, 255, 0}, 2);
    cv::line(inout, points[1], points[3], {0, 0, 255}, 2);
    cv::line(inout, points[2], points[3], {255, 0, 255}, 2);
}

/**
 * Write the H4 poinst value mentioned in the paper(section 3)
 * @param i_pts points of image before pertube
 * @param ip_pts points of image after pretube
 * @param out_file the file which save the points
 */
void write_h4pts(point_type const &i_pts, point_type const &ip_pts, std::ofstream &out_file)
{
    if(i_pts.size() != 4 || ip_pts.size() != 4){
        CV_Assert("i_pts.size() != 4 || ip_ptr.size() != 4");
    }
    for(size_t i = 0; i != 4; ++i){
       out_file<<(ip_pts[i].x - i_pts[i].x)<<"\t"<<(ip_pts[i].y - i_pts[i].y);
       if(i != 3){
           out_file<<"\t";
       }else{
           out_file<<"\n";
       }
    }
}

bool generate_image_pair(QString const &img_name,
                         QString const &save_at,
                         bool save_as_color,
                         std::ofstream &out_file,
                         std::uniform_int_distribution<int> &dist,
                         std::random_device &rd,
                         bool debug)
{
    cv::Mat img_i = cv::imread(img_name.toStdString()); //image I mentioned by the paper
    if(!img_i.empty()){
        if(img_i.rows < 240 || img_i.cols < 320){
            return false;
        }
        auto const origin_pts = random_crop(img_i.size(), 128, dist, rd);
        auto const pertube_pts = pertube_points(origin_pts, dist, rd);
        cv::Mat const hmat = cv::getPerspectiveTransform(origin_pts, pertube_pts).inv(cv::DECOMP_SVD);
        cv::Mat img_ip; //image I' mentioned by the paper
        if(!save_as_color){
            cv::cvtColor(img_i, img_i, CV_BGR2GRAY);
        }
        cv::warpPerspective(img_i, img_ip, hmat, img_i.size());

        if(debug){
            cv::Mat temp = img_i.clone();
            //check the bounding boxes looks reasonable or not
            draw_lines(origin_pts, temp);
            draw_lines(pertube_pts, temp);
            draw_lines(origin_pts, img_ip);

            cv::imshow("check boxes", temp);
            cv::imshow("check img warp", img_ip);
            cv::waitKey();
        }

        if(!debug){
            cv::Rect const rect(origin_pts[0], origin_pts[3]);
            std::string const base_name = QFileInfo(img_name).completeBaseName().toStdString();
            std::string const file_prefix = save_at.toStdString() + "/" + base_name;
            cv::imwrite(file_prefix + "_p.jpg", img_i(rect));
            cv::imwrite(file_prefix + "_pp.jpg", img_ip(rect));
            out_file<<(base_name + "_p.jpg")<<"\t"<<(base_name + "_pp.jpg")<<"\t";
            write_h4pts(origin_pts, pertube_pts, out_file);
        }

        return true;
    }else{
        qDebug()<<"cannot read img:"<<img_name;
        return false;
    }
}

