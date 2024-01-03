#include "widget_export_open_image_viewer_to_other_format.hpp"
#include "ui_widget_export_open_image_viewer_to_other_format.h"

#include <utils/qtable_item_utils.hpp>

#include <QCheckBox>
#include <QTableWidgetItem>

#include <QDir>
#include <QFile>
#include <QTextStream>

using namespace flt::ui;

widget_export_open_image_viewer_to_other_format::widget_export_open_image_viewer_to_other_format(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::widget_export_open_image_viewer_to_other_format)
{
    ui->setupUi(this);
}

widget_export_open_image_viewer_to_other_format::~widget_export_open_image_viewer_to_other_format()
{
    delete ui;
}

void widget_export_open_image_viewer_to_other_format::
    set_detector_info_list(std::map<QString, std::vector<open_image_detector_info> > *input)
{
    detector_info_list_ = input;
}

void widget_export_open_image_viewer_to_other_format::set_key_to_labels(std::map<QString, QString> *key_to_labels)
{
    key_to_labels_ = key_to_labels;
}

void widget_export_open_image_viewer_to_other_format::update_table(const QStringList &input)
{
    ui->tableWidgetSelectLabels->setRowCount(0);
    for(int i = 0; i != input.size(); ++i){
        ui->tableWidgetSelectLabels->insertRow(i);
        ui->tableWidgetSelectLabels->setItem(i, 0, new QTableWidgetItem(input[i]));
        ui->tableWidgetSelectLabels->setCellWidget(i, 1, create_checkbox());
        ui->tableWidgetSelectLabels->setItem(i, 2, new QTableWidgetItem(QString::number(i)));
    }

    ui->tableWidgetSelectLabels->resizeRowsToContents();
}

void widget_export_open_image_viewer_to_other_format::on_pushButtonExport_clicked()
{
    std::map<QString, int> select_labels;
    for(int i = 0; i != ui->tableWidgetSelectLabels->rowCount(); ++i){
        if(access_cell_widget<QCheckBox>(ui->tableWidgetSelectLabels->cellWidget(i, 1))->isChecked()){
            select_labels.emplace(ui->tableWidgetSelectLabels->item(i, 0)->text(), ui->tableWidgetSelectLabels->item(i, 2)->text().toInt());
        }
    }

    QDir().mkpath(ui->lineEditExportTo->text());
    for(auto const &val : *detector_info_list_){
        if(QFile file(ui->lineEditExportTo->text() + "/" + val.first + ".txt"); file.open(QIODevice::WriteOnly)){
            QTextStream stream(&file);
            for(auto const &info : val.second){
                if(auto it = key_to_labels_->find(info.label_); it != std::end(*key_to_labels_)){
                    if(auto sit = select_labels.find(it->second); sit != std::end(select_labels)){
                        auto const width = info.xmax_ - info.xmin_;
                        auto const height = info.ymax_ - info.ymin_;
                        auto const xcenter = info.xmin_ + width / 2;
                        auto const ycenter = info.ymin_ + height / 2;
                        stream<<sit->second<<" "<<xcenter<<" "<<ycenter<<" "<<width<<" "<<height<<"\n";
                    }
                }
            }
        }
    }
}

