#include "mainwindow.hpp"
#include "ui_mainwindow.h"

#include "dispatch_delegate.hpp"
#include "user_model.hpp"

#include <QDebug>

void MainWindow::init_model()
{
    model_.setItem(0, 0, new QStandardItem{"banana"});
    model_.setItem(1, 0, new QStandardItem{"pie"});
    model_.setItem(2, 0, new QStandardItem{"apple"});
    model_.setItem(3, 0, new QStandardItem{"bill"});
    model_.setItem(4, 0, new QStandardItem{"bolt"});
    model_.setItem(5, 0, new QStandardItem{"bany"});
    model_.setItem(6, 0, new QStandardItem{"tony"});
    model_.setItem(7, 0, new QStandardItem{"puny"});

    model_.setHorizontalHeaderItem(1, new QStandardItem{"waha"});

    model_.setData(model_.index(0, 1), 0, Qt::DisplayRole);
    model_.setData(model_.index(1, 1), 1, Qt::DisplayRole);
    model_.setData(model_.index(2, 1), 2, Qt::DisplayRole);
    model_.setData(model_.index(3, 1), 3, Qt::DisplayRole);
    model_.setData(model_.index(4, 1), 4, Qt::DisplayRole);
    model_.setData(model_.index(5, 1), 5, Qt::DisplayRole);
    model_.setData(model_.index(6, 1), 6, Qt::DisplayRole);
    model_.setData(model_.index(7, 1), 7, Qt::DisplayRole);

    ui->tableView->setModel(&model_);
    ui->tableViewNative->setModel(&model_);
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->menuOpen->setEnabled(false);

    /*auto delegate = new dispatch_delegate(this);
    auto model = new user_model(this);
    ui->tableView->setModel(model);
    ui->tableView->setItemDelegateForColumn(user_model::DispatchOption, delegate);*/

    init_model();

    //ui->tableView->verticalHeader()->setSectionsMovable(true);
    //ui->tableView->verticalHeader()->setDragEnabled(true);
    //ui->tableView->verticalHeader()->setDragDropMode(QAbstractItemView::DragDrop);
    //ui->tableView->verticalHeader()->setDragDropMode(QAbstractItemView::InternalMove);

    //ui->tableView->setDragEnabled(true);
    //ui->tableView->setAcceptDrops(true);
    //ui->tableView->viewport()->setAcceptDrops(true);
    ui->tableView->setDragDropOverwriteMode(false);
    //ui->tableView->setDropIndicatorShown(true);
    ui->tableView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setDragDropMode(QAbstractItemView::InternalMove);

    //ui->tableViewNative->setDragDropOverwriteMode(false);
    //ui->tableViewNative->setSelectionMode(QAbstractItemView::ExtendedSelection);
    //ui->tableViewNative->setSelectionBehavior(QAbstractItemView::SelectRows);
    //ui->tableViewNative->setDragDropMode(QAbstractItemView::InternalMove);

    connect(ui->tableView, SIGNAL(my_drop_action(int,QModelIndex,QVector<QVector<QVariant>>)),
            this, SLOT(drop_action_impl(int,QModelIndex,QVector<QVector<QVariant>>)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::drop_action_impl(int row,
                                  const QModelIndex &target,
                                  QVector<QVector<QVariant>> value)
{
    /*for(auto Data : value){
        qDebug()<<Data.toString();
    }

    qDebug()<<"row count after drop "<<model_.rowCount();
    for(int row = 0; row != model_.rowCount(); ++row){
        for(int col = 0; col != model_.columnCount(); ++col){
            qDebug()<<model_.data(model_.index(row, col), Qt::DisplayRole).toString();
        }
    }*/

    qDebug()<<"drop impl";
    if(target.isValid()){
        if(row >= target.row()){
            qDebug()<<"row >= target.row";
            auto target_row = target.row();
            model_.insertRows(target.row(), value.size());
            for(int i = 0; i != value.size(); ++i){
                //model_.insertRow(target.row());
                auto const &Vec = value[i];
                model_.setData(model_.index(target_row, 0), Vec[0].toString());
                model_.setData(model_.index(target_row, 1), Vec[1].toInt());
                ++target_row;
            }
        }else if(row < target.row()){
            qDebug()<<"row < target.row";
            auto target_row = target.row() + 1;
            model_.insertRows(target_row, value.size());
            for(int i = 0; i != value.size(); ++i){
                auto const &Vec = value[i];
                model_.setData(model_.index(target_row, 0), Vec[0].toString());
                model_.setData(model_.index(target_row, 1), Vec[1].toInt());
                ++target_row;
            }
        }
    }else{
        qDebug()<<"insert data";
        int target_row = model_.rowCount();
        model_.insertRows(target_row, value.size());
        for(int i = 0; i != value.size(); ++i){
            auto const &Vec = value[i];
            model_.setData(model_.index(target_row, 0), Vec[0].toString());
            model_.setData(model_.index(target_row, 1), Vec[1].toInt());
            ++target_row;
        }
    }
}

void MainWindow::on_pushButton_clicked()
{
    for(int i = 0; i != model_.rowCount(); ++i){
        if(model_.item(i, 0)){
            qDebug()<<model_.item(i, 0)->data(Qt::DisplayRole).value<QString>();
        }
    }
    ui->tableView->reset();
    init_model();
}

void MainWindow::on_pushButtonNative_clicked()
{

}
