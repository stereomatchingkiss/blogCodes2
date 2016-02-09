//#include "general_setting.hpp"
#include "object_classify_setting.hpp"

#include "setting_window.hpp"
#include "ui_setting_window.h"

#include "../utility/global.hpp"

#include <QDebug>
#include <QStringListModel>

setting_window::setting_window(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::setting_window),
    obj_classify_setting_(new object_classify_setting(this))
{
    ui->setupUi(this);    
    ui->stackedWidgetSetting->addWidget(obj_classify_setting_);
    //ui->stackedWidgetSetting->addWidget(general_setting_);

    auto *model = new QStringListModel(this);
    model->setStringList(global::algo_list);
    ui->listViewAlgo->setModel(model);
    ui->listViewAlgo->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->listViewAlgo->setCurrentIndex(model->index(0,0));

    connect(ui->listViewAlgo, SIGNAL(clicked(QModelIndex)),
            this, SLOT(algo_view_clicked(QModelIndex)));
}

setting_window::~setting_window()
{
    delete ui;
}

void setting_window::algo_view_clicked(QModelIndex index)
{
    qDebug()<<"click on row "<<index.row();
    ui->stackedWidgetSetting->setCurrentIndex(index.row());
}
