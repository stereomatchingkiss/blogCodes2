#include "convert_label_img_labels.hpp"
#include "ui_convert_label_img_labels.h"

#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QFileInfo>
#include <QImageReader>
#include <QTextStream>

#include <QDomDocument>

namespace{

struct parse_data
{
    struct object
    {
        QString label_;
        QRectF rect_;
    };

    QString abs_file_path_;
    std::vector<object> objects_;

    void print() const
    {
        qDebug()<<__func__<<": file path = "<<abs_file_path_;
        for(auto const &obj : objects_){
            qDebug()<<__func__<<": label = "<<obj.label_<<", rect = "<<obj.rect_;
        }
    }
};

QRectF parse_bndbox(QDomElement &elem)
{
    QRectF rect;
    while(!elem.isNull()){
        if(elem.tagName() == "xmin"){
            rect.setX(elem.firstChild().toText().data().toDouble());
        }else if(elem.tagName() == "ymin"){
            rect.setY(elem.firstChild().toText().data().toDouble());
        }else if(elem.tagName() == "xmax"){
            rect.setRight(elem.firstChild().toText().data().toDouble());
        }else{
            rect.setBottom(elem.firstChild().toText().data().toDouble());
        }
        elem = elem.nextSibling().toElement();
    }

    return rect;
}

parse_data parse_xml(QString const &fname)
{
    QFile file(QFileInfo(fname).absoluteFilePath());
    parse_data result;
    if(file.open(QIODevice::ReadOnly)){
        QDomDocument doc;
        doc.setContent(&file);
        auto dom_elem = doc.firstChild().toElement().firstChild().toElement();
        while(!dom_elem.isNull()){
            if(dom_elem.tagName() == "path"){
                result.abs_file_path_ = dom_elem.firstChild().toText().data();
            }else if(dom_elem.tagName() == "object"){
                parse_data::object obj;
                auto child = dom_elem.firstChild().toElement();
                while(!child.isNull()){
                    if(child.tagName() == "name"){
                        obj.label_ = child.firstChild().toText().data();
                    }else if(child.tagName() == "bndbox"){
                        auto bndbox = child.firstChild().toElement();
                        obj.rect_ = parse_bndbox(bndbox);
                    }

                    child = child.nextSibling().toElement();
                }
                result.objects_.emplace_back(obj);
            }

            dom_elem = dom_elem.nextSibling().toElement();
        }
    }else{
        qDebug()<<__func__<<"cannot open file = "<<file.fileName();
    }
    return result;
}

void to_alexeyab_yolo_format(std::vector<parse_data> const &input_vec, std::map<QString, int> const &labels_to_int)
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
    ui->tableWidget->insertRow(ui->tableWidget->rowCount() > 0 ? ui->tableWidget->rowCount() - 1 : 0);
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
    std::vector<parse_data> parse_data_vec;
    int index = 0;
    for(auto const &fname : file_paths_){
        qDebug()<<__func__<<" parse fname = "<<fname;
        auto parse_result = parse_xml(fname);
        for(auto const &val : parse_result.objects_){
            if(auto it = labels_to_int.find(val.label_); it == std::end(labels_to_int)){
                labels_to_int.insert({val.label_, index++});
            }
        }
        parse_data_vec.emplace_back(std::move(parse_result));
    }
    to_alexeyab_yolo_format(parse_data_vec, labels_to_int);
}
