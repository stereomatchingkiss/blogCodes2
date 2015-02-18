#include "drag_drop.hpp"
#include "ui_drag_drop.h"

#include <QDebug>

drag_drop::drag_drop(QWidget *parent) :
    QDialog(parent),
    left_model_(QStringList()<<"baba"<<"doremi"<<"onpu"<<"majo rika"),
    right_model_(QStringList()<<"dojimi"<<"hana"<<"terry"<<"kimi"<<"nana"),
    ui(new Ui::drag_drop)
{
    ui->setupUi(this);
    ui->listViewLeft->setModel(&left_model_);
    ui->listViewRight->setModel(&right_model_);

    connect(ui->listViewLeft,
            SIGNAL(my_drop_action(int,QModelIndex,QString,int)),
            this,
            SLOT(drop_action(int,QModelIndex,QString,int)));

    connect(ui->listViewRight,
            SIGNAL(my_drop_action(int,QModelIndex,QString,int)),
            this,
            SLOT(drop_action(int,QModelIndex,QString,int)));
}

drag_drop::~drag_drop()
{
    delete ui;
}

void drag_drop::on_pushButtonPrint_clicked()
{
    left_model_.setStringList(QStringList()<<"baba"<<"doremi"<<"onpu"<<"majo rika");
    right_model_.setStringList(QStringList()<<"dojimi"<<"hana"<<"terry"<<"kimi"<<"nana");
}

void drag_drop::drop_action(int row,
                            QModelIndex const &target,
                            QString const &text,
                            int)
{
    if(target.model() == &left_model_){
        drop_action_impl(row, target, text, left_model_);
    }else{
        drop_action_impl(row, target, text, right_model_);
    }
}

void drag_drop::drop_action_impl(int row,
                                 const QModelIndex &target,
                                 const QString &text,
                                 QStringListModel &model)
{
    if(row >= target.row()){
        model.insertRow(target.row());
        model.setData(target, text);
    }else if(target.model() == &model && row < target.row()){
        model.insertRow(target.row() + 1);
        model.setData(model.index(target.row() + 1, 0), text);
    }
}
