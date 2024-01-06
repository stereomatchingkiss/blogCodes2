#include "widget_move_files_to_other_folder.hpp"
#include "ui_widget_move_files_to_other_folder.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>

widget_move_files_to_other_folder::widget_move_files_to_other_folder(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::widget_move_files_to_other_folder)
{
    ui->setupUi(this);
}

widget_move_files_to_other_folder::~widget_move_files_to_other_folder()
{
    delete ui;
}

void widget_move_files_to_other_folder::on_pushButtonMove_clicked()
{
    QString const source_folder(ui->lineEditSource->text());
    QString const target_folder(ui->lineEditTarget->text());
    QDir().mkpath(target_folder);
    auto const finfo_list = QDir(QString("%1").arg(source_folder)).entryInfoList();
    for(int i = 0; i < static_cast<int>(finfo_list.size() * (ui->spinBoxRatio->value() / 100.0)); ++i){
        QDir().rename(finfo_list[i].absoluteFilePath(), QString("%1/%2").arg(target_folder, finfo_list[i].fileName()));
    }
}

