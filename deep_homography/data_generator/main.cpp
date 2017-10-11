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

/**
 * Collection of data parsed from command line
 *
 * @param debug_ true will print/show debug message/image and vice versa
 * @param max_size_ maximum number of the image want to generate
 * @param save_at_ where to save the generated images
 * @param save_as_color_ true, save image as color image, false save as gray image
 * @param out_file_ this file will save the corners points of associate image
 * @param pertube_dist_ uniform int distribution to pretube the points of input
 * @param rd_ random number generator of dist
 * @param save_warped_image_ save the image after resize and warped and vice versa
 */
struct parsed_data
{
    parsed_data();

    bool debug_ = false;
    size_t max_size_ = 1000;
    std::ofstream out_file_;
    std::uniform_int_distribution<int> pertube_dist_;
    std::random_device rd_;
    bool save_as_color_ = false;
    bool save_warped_image_ = false;
    QString save_at_;
};

parsed_data::parsed_data() : pertube_dist_(-32, 32)
{

}


void create_parser(QCommandLineParser &parser);

/**
 * generate_image_pair generate a pair of images
 * mentioned by section 3 of the paper and a file
 * called info.txt, this file record the info of
 * generated image with format
 *
 * name of patch I\tname of patch I'\tdelta x1\tdelta y1\t
 * delta x2\tdelta y2\tdelta x3\tdelta y3\tdelta x4\tdelta y4\t
 * x1 of patch I\ty1 of patch I\tx2 of patch I\ty2 of patch I\t
 * x3 of patch I\ty3 of patch I\tx4 of patch I\ty4 of patch I\n
 *
 * delta (x1, y1) ~ delta (x4, y4) are the delta mentioned by
 * the paper. (x1,y1)~(x4,y4) are the original coordinate(coordinate without pertube)
 * All of them sort by the order top left, top right, bottom left, bottom right
 *
 * If you set save_warped_image as true, every line will add the
 * image name before warped and wpared(ex : 01_bp.jpg\t01_bpp.jpg\n)
 *
 * @param img_name name of the image
 * @param inout This struct save the settings
 * @return true if image pair can be generated and vice versa
 */
bool generate_image_pair(QString const &img_name,
                         parsed_data &pd);

void parse_command_line(QCommandLineParser const &parser, parsed_data &inout);

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
    size_t gen_image_size = 0;
    parsed_data pd;
    parse_command_line(parser, pd);
    while(dir.hasNext()){        
        if(pd.debug_ && i == 10){
            qDebug()<<"reach debug size";
            break;
        }
        QFileInfo const info(dir.next());
        qDebug()<<i++<<":"<<info.fileName();
        bool const can_gen_image = generate_image_pair(info.absoluteFilePath(), pd);
        if(can_gen_image){
            ++gen_image_size;
            if(gen_image_size >= pd.max_size_){
                qDebug()<<"reach max size";
                break;
            }
        }
    }

    return 0;
}catch(std::exception const &ex){
    qDebug()<<ex.what();
}

using point_type = std::array<cv::Point2f, 4>;

void parse_command_line(QCommandLineParser const &parser, parsed_data &inout)
{
    inout.debug_ = parser.value("debug").compare("false", Qt::CaseInsensitive) == 0 ? false : true;
    inout.max_size_ = parser.value("max_size").toInt();
    inout.save_at_ = parser.value("output");
    inout.out_file_.open((parser.value("output") + "/info.txt").toStdString());
    inout.save_as_color_ = parser.value("debug").compare("false", Qt::CaseInsensitive) == 0 ? false : true;
    inout.save_warped_image_ = parser.value("save_warped_image").compare("false", Qt::CaseInsensitive) == 0 ? false : true;
}

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
                      "The place of saving generated images and information",
                      "output"});
    parser.addOption({{"d", "debug"},
                      "True will print/show debug message/image, false will not. Default value is false",
                      "debug", "false"});
    parser.addOption({{"c", "color"},
                      "True will save generated image as color image(b,g,r), false will be gray image.\n"
                      "Default value is false",
                      "color", "false"});
    parser.addOption({{"p", "save_warped_image"},
                      "True will save the image after resize and warped and vice versa.\n"
                      "Default value is false. Save image will have postfix bp.jpg bpp.jpg.",
                      "save_warped_image", "false"});
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
void write_points(point_type const &i_pts, point_type const &ip_pts, std::ofstream &out_file)
{
    if(i_pts.size() != 4 || ip_pts.size() != 4){
        CV_Assert("i_pts.size() != 4 || ip_ptr.size() != 4");
    }
    for(size_t i = 0; i != 4; ++i){
       out_file<<(ip_pts[i].x - i_pts[i].x)<<"\t"<<(ip_pts[i].y - i_pts[i].y)<<"\t";
    }
    for(size_t i = 0; i != 4; ++i){
        out_file<<i_pts[i].x<<"\t"<<i_pts[i].y<<"\t";
    }
}

bool generate_image_pair(QString const &img_name, parsed_data &pd)
{
    cv::Mat img_i = cv::imread(img_name.toStdString()); //image I mentioned by the paper
    if(!img_i.empty()){
        cv::resize(img_i, img_i, {320, 240});
        auto const origin_pts = random_crop(img_i.size(), 128, pd.pertube_dist_, pd.rd_);
        auto const pertube_pts = pertube_points(origin_pts, pd.pertube_dist_, pd.rd_);
        cv::Mat const hmat = cv::getPerspectiveTransform(origin_pts, pertube_pts).inv(cv::DECOMP_SVD);                

        cv::Mat img_ip; //image I' mentioned by the paper
        if(pd.save_warped_image_){
            std::string const base_name = QFileInfo(img_name).completeBaseName().toStdString();
            std::string const file_prefix = pd.save_at_.toStdString() + "/" + base_name;
            cv::imwrite(file_prefix + "_bp.jpg", img_i);
            cv::warpPerspective(img_i, img_ip, hmat, img_i.size());
            cv::imwrite(file_prefix + "_bpp.jpg", img_ip);
        }

        if(!pd.save_as_color_){
            cv::cvtColor(img_i, img_i, CV_BGR2GRAY);
        }
        cv::warpPerspective(img_i, img_ip, hmat, img_i.size());

        if(pd.debug_){
            cv::Mat temp = img_i.clone();
            //check the bounding boxes looks reasonable or not
            draw_lines(origin_pts, temp);
            draw_lines(pertube_pts, temp);
            draw_lines(origin_pts, img_ip);

            cv::imshow("check boxes", temp);
            cv::imshow("check img warp", img_ip);
            cv::waitKey();
        }

        if(!pd.debug_){
            cv::Rect const rect(origin_pts[0], origin_pts[3]);
            std::string const base_name = QFileInfo(img_name).completeBaseName().toStdString();
            std::string const file_prefix = pd.save_at_.toStdString() + "/" + base_name;
            cv::imwrite(file_prefix + "_p.jpg", img_i(rect));
            cv::imwrite(file_prefix + "_pp.jpg", img_ip(rect));            
            pd.out_file_<<(base_name + "_p.jpg")<<"\t"<<(base_name + "_pp.jpg")<<"\t";
            write_points(origin_pts, pertube_pts, pd.out_file_);
            pd.out_file_<<(base_name + "_bp.jpg")<<"\t"<<(base_name + "_bpp.jpg")<<"\n";
        }

        return true;
    }else{
        qDebug()<<"cannot read img:"<<img_name;
        return false;
    }
}
