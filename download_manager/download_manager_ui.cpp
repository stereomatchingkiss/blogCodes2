#include "download_manager_ui.hpp"
#include "ui_download_manager_ui.h"

download_manager_ui::download_manager_ui(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::download_manager_ui)
{
    ui->setupUi(this);
}

download_manager_ui::~download_manager_ui()
{
    delete ui;
}
