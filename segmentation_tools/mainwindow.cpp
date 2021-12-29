#include "mainwindow.hpp"
#include "ui_mainwindow.h"

#include "color_filter_widget.hpp"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->stackedWidget->addWidget(new color_filter_widget);
}

MainWindow::~MainWindow()
{
    delete ui;
}

