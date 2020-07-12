#include "convert_label_img_labels.hpp"
#include "ui_convert_label_img_labels.h"

#include "parser_label_img.hpp"

#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QFileInfo>
#include <QImageReader>
#include <QTextStream>

#include <QDomDocument>

namespace{

void to_alexeyab_yolo_format(std::vector<parser_label_img::parse_data> const &input_vec,
                             std::map<QString, int> const &labels_to_int)
{
    for(auto const &input: input_vec){
        QFileInfo finfo(input.abs_file_path_);
        QFile file(finfo.absolutePath() + "/" +  finfo.baseName() +  ".txt");
        if(file.open(QIODevice::WriteOnly)){
            QImageReader im_reader(input.abs_file_path_);
            if(im_reader.canRead()){
                auto const im_size = im_reader.size();
                QTextStream stream(&file);
                for(auto const &data : input.objects_){
                    if(auto it = labels_to_int.find(data.label_); it != std::end(labels_to_int)){
                        auto const center = data.rect_.center();
                        stream<<it->second<<" "<<center.x()/im_size.width()<<" "<<center.y()/im_size.height()<<" ";
                        stream<<data.rect_.width()/im_size.width()<<" "<<data.rect_.height()/im_size.height()<<"\n";
                    }
                }
            }else{
                qDebug()<<__func__<<": cannot read image = "<<input.abs_file_path_;
            }
        }
    }
}

void generate_img_list(std::vector<parser_label_img::parse_data> const &input_vec,
                       QString const &save_img_list_as)
{
    QFile file(save_img_list_as);
    if(file.open(QFile::WriteOnly)){
        QTextStream stream(&file);
        for(auto const &val: input_vec){
            stream<<val.abs_file_path_<<"\n";
        }
    }else{
        qDebug()<<__func__<<": cannot save img_list as = "<<save_img_list_as;
    }
}

}

convert_label_img_labels::convert_label_img_labels(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::convert_label_img_labels)
{
    ui->setupUi(this);

    ui->comboBoxToFormat->addItem(tr("AlexeyAB yolo"));
}

convert_label_img_labels::~convert_label_img_labels()
{
    delete ui;
}

void convert_label_img_labels::on_pushButtonAdd_clicked()
{
    ui->tableWidget->insertRow(ui->tableWidget->rowCount());
}

void convert_label_img_labels::on_pushButtonRemove_clicked()
{
    if(ui->tableWidget->rowCount() > 0){
        ui->tableWidget->removeRow(ui->tableWidget->rowCount() - 1);
    }
}

void convert_label_img_labels::on_pushButtonConvert_clicked()
{
    file_paths_.clear();
    for(int i = 0; i != ui->tableWidget->rowCount(); ++i){
        QDirIterator it(ui->tableWidget->item(i, 0)->text(), QStringList()<<"*.xml",
                        QDir::NoDotAndDotDot | QDir::Files | QDir::Dirs, QDirIterator::NoIteratorFlags);
        while(it.hasNext()){
            file_paths_.emplace_back(it.next());
        }
    }

    qDebug()<<__func__<<" file paths size = "<<file_paths_.size();
    std::map<QString, int> labels_to_int;
    std::vector<parser_label_img::parse_data> parse_data_vec;
    parser_label_img parser;
    int index = 0;
    for(auto const &fname : file_paths_){
        qDebug()<<__func__<<" parse fname = "<<fname;
        auto parse_result = parser.parse(fname);
        for(auto const &val : parse_result.objects_){
            if(auto it = labels_to_int.find(val.label_); it == std::end(labels_to_int)){
                labels_to_int.insert({val.label_, index++});
            }
        }
        parse_data_vec.emplace_back(std::move(parse_result));
    }
    to_alexeyab_yolo_format(parse_data_vec, labels_to_int);
    generate_img_list(parse_data_vec, ui->lineEditSaveImgListAs->text());
}
