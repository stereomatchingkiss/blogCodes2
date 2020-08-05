#include "labels_check.hpp"
#include "ui_labels_check.h"

#include "parser_label_img.hpp"

#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QFileInfo>

#include <set>

namespace{

QStringList get_files_path(QString const &folder, QStringList const &suffix_filter)
{
    QDirIterator dir_it(folder, suffix_filter,
                        QDir::NoDotAndDotDot | QDir::Files | QDir::Dirs,
                        QDirIterator::NoIteratorFlags);
    QStringList results;
    while(dir_it.hasNext()){
        results.push_back(QDir::toNativeSeparators(QFileInfo(dir_it.next()).absoluteFilePath()));
    }

    return results;
}

void insert_item_if_sec_set_do_not_has_first_set_item(std::set<QString> const &first_set,
                                                      std::set<QString> const &sec_set,
                                                      QTableWidget *table)
{
    int row = 0;
    for(auto it = first_set.begin(); it != first_set.end(); ++it){
        if(auto it2 = sec_set.find(*it); it2 == sec_set.end()){
            table->insertRow(row);
            table->setItem(row, 0, new QTableWidgetItem(*it));
            ++row;
        }
    }
    table->resizeColumnsToContents();
    table->resizeRowsToContents();
}

}

labels_check::labels_check(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::labels_check)
{
    ui->setupUi(this);
}

labels_check::~labels_check()
{
    delete ui;
}

void labels_check::on_pushButtonCheck_clicked()
{
    auto images_path = get_files_path(ui->lineEditImageFolder->text(),
                                      QStringList()<<"*.jpg"<<"*.png"<<"*.bmp"<<"*.tiff"<<"*.jpeg");
    auto labels_path = get_files_path(ui->lineEditLabelFolder->text(),
                                      QStringList()<<"*.xml");

    std::set<QString> im_path_in_label;
    for(auto const &lpath : labels_path){
        im_path_in_label.insert(QFileInfo(parser_label_img().parse(lpath).abs_file_path_).fileName());
    }

    std::set<QString> im_path_set;
    for(auto const &im_path : images_path){
        im_path_set.insert(QFileInfo(im_path).fileName());
    }

    ui->tableWidgetMissImg->setRowCount(0);
    ui->tableWidgetMissLabel->setRowCount(0);

    insert_item_if_sec_set_do_not_has_first_set_item(im_path_set, im_path_in_label, ui->tableWidgetMissLabel);
    insert_item_if_sec_set_do_not_has_first_set_item(im_path_in_label, im_path_set, ui->tableWidgetMissImg);
}
