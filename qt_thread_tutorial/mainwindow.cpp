#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QThread>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    controller_(new naive_controller(this)),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButtonPrintCurThread_clicked()
{
    qDebug()<<QThread::currentThread();
}

void MainWindow::on_pushButtonCallNormalFunc_clicked()
{
    controller_->print_working_thread_by_normal_call();
}

void MainWindow::on_pushButtonCallSignalAndSlot_clicked()
{
    controller_->print_working_thread_by_signal_and_slot();
}
