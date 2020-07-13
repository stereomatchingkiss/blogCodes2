#include "generate_image_list.hpp"
#include "ui_generate_image_list.h"

#include "utils/iterator_create.hpp"

#include <QFile>
#include <QTextStream>

generate_image_list::generate_image_list(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::generate_image_list)
{
    ui->setupUi(this);
}

generate_image_list::~generate_image_list()
{
    delete ui;
}

void generate_image_list::on_pushButtonAddDir_clicked()
{
    ui->tableWidget->insertRow(ui->tableWidget->rowCount());
}

void generate_image_list::on_pushButtonRemoveDir_clicked()
{
    if(ui->tableWidget->rowCount() > 0){
        ui->tableWidget->removeRow(ui->tableWidget->rowCount() - 1);
    }
}

void generate_image_list::on_pushButtonGenerate_clicked()
{
    if(ui->tableWidget->rowCount() > 0 && !ui->lineEditSaveImageAs->text().isEmpty()){
        QStringList paths;
        for(int i = 0; i != ui->tableWidget->rowCount(); ++i){
            auto it = create_image_iterator(ui->tableWidget->item(i, 0)->text());
            while(it.hasNext()){
                paths.push_back(it.next());
            }
        }

        QFile file(ui->lineEditSaveImageAs->text());
        if(file.open(QIODevice::WriteOnly)){
            QTextStream stream(&file);
            for(auto const &val : paths){
                stream<<val<<"\n";
            }
        }
    }
}
