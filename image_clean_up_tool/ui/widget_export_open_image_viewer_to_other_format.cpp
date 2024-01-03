#include "widget_export_open_image_viewer_to_other_format.hpp"
#include "ui_widget_export_open_image_viewer_to_other_format.h"

#include <json/json_utils.hpp>
#include <utils/qtable_item_utils.hpp>

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

#include <QCheckBox>
#include <QTableWidgetItem>

#include <QDir>
#include <QFile>
#include <QSettings>
#include <QTextStream>

using namespace flt::ui;
using namespace flt::json;

namespace{

QString const state_open_image_export_to("state_open_image_export_to");
QString const state_open_image_load_selected_items("state_open_image_load_selected_items");
QString const state_open_image_save_selected_items("state_open_image_save_selected_items");

}

widget_export_open_image_viewer_to_other_format::widget_export_open_image_viewer_to_other_format(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::widget_export_open_image_viewer_to_other_format)
{
    ui->setupUi(this);

    ui->comboBoxFormat->addItem("yolov5");

    QSettings settings;
    if(settings.contains(state_open_image_export_to)){
        ui->lineEditExportTo->setText(settings.value(state_open_image_export_to).toString());
    }
    if(settings.contains(state_open_image_load_selected_items)){
        ui->lineEditLoadSelectedItems->setText(settings.value(state_open_image_load_selected_items).toString());
    }
    if(settings.contains(state_open_image_save_selected_items)){
        ui->lineEditSaveSelectedItems->setText(settings.value(state_open_image_save_selected_items).toString());
    }
}

widget_export_open_image_viewer_to_other_format::~widget_export_open_image_viewer_to_other_format()
{
    QSettings settings;
    settings.setValue(state_open_image_export_to, ui->lineEditExportTo->text());
    settings.setValue(state_open_image_load_selected_items, ui->lineEditLoadSelectedItems->text());
    settings.setValue(state_open_image_save_selected_items, ui->lineEditSaveSelectedItems->text());

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
        ui->tableWidgetSelectLabels->setCellWidget(i, 3, create_checkbox());
    }

    ui->tableWidgetSelectLabels->resizeRowsToContents();
}

void widget_export_open_image_viewer_to_other_format::on_pushButtonExport_clicked()
{
    std::map<QString, std::tuple<bool, int>> select_labels;
    bool must_have_exist = false;
    for(int i = 0; i != ui->tableWidgetSelectLabels->rowCount(); ++i){
        if(access_cell_widget<QCheckBox>(ui->tableWidgetSelectLabels->cellWidget(i, 1))->isChecked()){
            bool const must_have =
                access_cell_widget<QCheckBox>(ui->tableWidgetSelectLabels->cellWidget(i, 3))->isChecked();
            select_labels.emplace(ui->tableWidgetSelectLabels->item(i, 0)->text(),
                                  std::make_tuple(must_have,
                                                  ui->tableWidgetSelectLabels->item(i, 2)->text().toInt()));
            if(must_have){
                must_have_exist = true;
            }
        }
    }

    QDir().mkpath(ui->lineEditExportTo->text());
    for(auto const &val : *detector_info_list_){
        std::vector<std::tuple<open_image_detector_info, int>> valid_labels;
        bool must_have_valid = must_have_exist ? false : true;
        for(auto const &info : val.second){
            if(auto it = key_to_labels_->find(info.label_); it != std::end(*key_to_labels_)){
                if(auto sit = select_labels.find(it->second); sit != std::end(select_labels)){
                    valid_labels.emplace_back(info, std::get<1>(sit->second));
                    if(std::get<0>(sit->second)){
                        must_have_valid = true;
                    }
                }
            }
        }

        if(!valid_labels.empty() && must_have_valid){
            if(QFile file(ui->lineEditExportTo->text() + "/" + val.first + ".txt"); file.open(QIODevice::WriteOnly)){
                QTextStream stream(&file);
                for(auto const &[info, index] : valid_labels){
                    auto const width = info.xmax_ - info.xmin_;
                    auto const height = info.ymax_ - info.ymin_;
                    auto const xcenter = info.xmin_ + width / 2;
                    auto const ycenter = info.ymin_ + height / 2;
                    stream<<index<<" "<<xcenter<<" "<<ycenter<<" "<<width<<" "<<height<<"\n";
                }
            }
        }
    }
}


void widget_export_open_image_viewer_to_other_format::on_pushButtonSaveSelectedItems_clicked()
{    
    QJsonArray arr;
    for(int i = 0; i != ui->tableWidgetSelectLabels->rowCount(); ++i){
        if(access_cell_widget<QCheckBox>(ui->tableWidgetSelectLabels->cellWidget(i, 1))->isChecked()){
            QJsonObject obj;
            obj["index"] = ui->tableWidgetSelectLabels->item(i, 2)->text().toInt();
            obj["label"] = ui->tableWidgetSelectLabels->item(i, 0)->text();
            obj["must_have"] = access_cell_widget<QCheckBox>(ui->tableWidgetSelectLabels->cellWidget(i, 3))->isChecked();
            arr.push_back(obj);
        }
    }

    QJsonObject obj;
    obj["arr"] = arr;
    write_file_to_json(obj, ui->lineEditSaveSelectedItems->text());
}

void widget_export_open_image_viewer_to_other_format::on_pushButtonLoadSelectedItems_clicked()
{
    auto const obj = parse_file_to_jobj(ui->lineEditLoadSelectedItems->text());
    QJsonArray const arr = obj["arr"].toArray();

    std::map<QString, std::tuple<bool, int>> arr_map;
    for(int i = 0; i != arr.size(); ++i){
        auto const temp = arr[i].toObject();
        std::tuple<bool, int> val;
        std::get<1>(val) = temp["index"].toInt();
        if(temp.contains("must_have")){
            std::get<0>(val) = temp["must_have"].toBool();
        }else{
            std::get<0>(val) = false;
        }
        arr_map.emplace(temp["label"].toString(), val);
    }

    for(int i = 0; i != ui->tableWidgetSelectLabels->rowCount(); ++i){
        auto *cbox = access_cell_widget<QCheckBox>(ui->tableWidgetSelectLabels->cellWidget(i, 1));
        cbox->setChecked(false);
        if(auto it = arr_map.find(ui->tableWidgetSelectLabels->item(i, 0)->text()); it != std::end(arr_map)){
            cbox->setChecked(true);
            ui->tableWidgetSelectLabels->setItem(i, 2, new QTableWidgetItem(QString::number(std::get<1>(it->second))));
            access_cell_widget<QCheckBox>(ui->tableWidgetSelectLabels->cellWidget(i, 3))->setChecked(std::get<0>(it->second));
        }
    }
}

void widget_export_open_image_viewer_to_other_format::on_radioButtonShowSelectedItems_clicked()
{
    if(ui->radioButtonShowSelectedItems->isChecked()){
        for(int i = 0; i != ui->tableWidgetSelectLabels->rowCount(); ++i){
            if(!access_cell_widget<QCheckBox>(ui->tableWidgetSelectLabels->cellWidget(i, 1))->isChecked()){
                ui->tableWidgetSelectLabels->hideRow(i);
            }
        }
    }else{
        for(int i = 0; i != ui->tableWidgetSelectLabels->rowCount(); ++i){
            ui->tableWidgetSelectLabels->showRow(i);
        }
    }
}

