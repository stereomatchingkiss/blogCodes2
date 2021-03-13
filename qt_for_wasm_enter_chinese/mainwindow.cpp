#include "mainwindow.hpp"
#include "ui_mainwindow.h"

#include "font_manager.hpp"

#include <QByteArray>
#include <QDebug>
#include <QFile>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QFont const monospace(font_manager().get_font_family());
    ui->plainTextEdit->setFont(monospace);
}

MainWindow::~MainWindow()
{
    delete ui;
}
