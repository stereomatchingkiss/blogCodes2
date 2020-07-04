#include "mainwindow.hpp"
#include "ui_mainwindow.h"

#include "image_mover.hpp"
#include "remove_duplicate_images.hpp"
#include "split_data.hpp"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->tabWidget->addTab(new image_mover, tr("move image"));
    ui->tabWidget->addTab(new remove_duplicate_images, tr("Remove duplicate images"));
    ui->tabWidget->addTab(new split_data, tr("Split data"));
}

MainWindow::~MainWindow()
{
    delete ui;
}

