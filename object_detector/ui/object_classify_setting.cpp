#include "object_classify_setting.hpp"
#include "ui_object_classify_setting.h"

object_classify_setting::object_classify_setting(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::object_classify_setting)
{
    ui->setupUi(this);
}

object_classify_setting::~object_classify_setting()
{
    delete ui;
}
