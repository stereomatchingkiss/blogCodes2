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

    ui->listViewLeft->set_name("list view left");
    ui->listViewRight->set_name("list view right");

    ui->listViewLeft->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->listViewRight->setSelectionMode(QAbstractItemView::ExtendedSelection);

    ui->listViewLeft->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->listViewLeft, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(handle_custom_context(QPoint)));

    connect(ui->listViewLeft,
            SIGNAL(my_drop_action(int,QModelIndex,QStringList)),
            this,
            SLOT(drop_action_from_left(int,QModelIndex,QStringList)));

    connect(ui->listViewRight,
            SIGNAL(my_drop_action(int,QModelIndex,QStringList)),
            this,
            SLOT(drop_action_from_right(int,QModelIndex,QStringList)));
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

void drag_drop::drop_action_from_left(int row,
                                      QModelIndex const &target,
                                      QStringList const &text)
{    
    qDebug()<<__FUNCTION__;
    drop_action_impl(row, target, text, left_model_);
}

void drag_drop::drop_action_from_right(int row,
                                       QModelIndex const &target,
                                       QStringList const &text)
{
    qDebug()<<__FUNCTION__;
    drop_action_impl(row, target, text, right_model_);
}

void drag_drop::handle_custom_context(const QPoint &point)
{
    qDebug()<<point;
}

void drag_drop::drop_action_impl(int row,
                                 const QModelIndex &target,
                                 const QStringList &text,
                                 QStringListModel &model)
{            
    qDebug()<<"source row : "<<row;
    qDebug()<<"drop impl";
    if(target.isValid()){
        if(row >= target.row()){
            qDebug()<<"row >= target.row";            
            int target_row = target.row();
            model.insertRows(target.row(), text.size());
            for(int i = 0; i != text.size(); ++i){
                model.setData(model.index(target_row, 0), text[i]);
                ++target_row;
            }
        }else if(row < target.row()){
            qDebug()<<"row < target.row";
            int target_row = target.row() + 1;
            model.insertRows(target_row, text.size());
            for(int i = 0; i != text.size(); ++i){
                model.setData(model.index(target_row, 0), text[i]);
                ++target_row;
            }
        }
    }else{
        qDebug()<<"insert data";        
        int target_row = model.rowCount();
        model.insertRows(target_row, text.size());
        for(int i = 0; i != text.size(); ++i){
            model.setData(model.index(target_row, 0), text[i]);
            ++target_row;
        }
    }
}
