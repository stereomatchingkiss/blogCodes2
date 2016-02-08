#include "general_setting.hpp"
#include "ui_general_setting.h"

general_setting::general_setting(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::general_setting)
{
    ui->setupUi(this);
}

general_setting::~general_setting()
{
    delete ui;
}
