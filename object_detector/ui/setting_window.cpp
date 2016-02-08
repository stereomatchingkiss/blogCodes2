//#include "general_setting.hpp"
#include "setting_window.hpp"
#include "ui_setting_window.h"

#include "../utility/global.hpp"

#include <QStringListModel>

setting_window::setting_window(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::setting_window)
{
    ui->setupUi(this);
    //ui->stackedWidgetSetting->addWidget(general_setting_);

    auto *model = new QStringListModel(this);
    model->setStringList(global::algo_list);
    ui->listViewAlgo->setModel(model);    
}

setting_window::~setting_window()
{
    delete ui;
}
