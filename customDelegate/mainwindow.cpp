#include "mainwindow.hpp"
#include "ui_mainwindow.h"

#include "dispatch_delegate.hpp"
#include "user_model.hpp"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->menuOpen->setEnabled(false);

    auto delegate = new dispatch_delegate(this);
    auto model = new user_model(this);
    ui->tableView->setModel(model);
    ui->tableView->setItemDelegateForColumn(user_model::DispatchOption, delegate);
}

MainWindow::~MainWindow()
{
    delete ui;
}
