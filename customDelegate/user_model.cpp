#include "user_model.hpp"

#include <QDebug>
#include <QMessageBox>
#include <QModelIndex>

user_model::user_model(QObject *obj) : QStandardItemModel(obj)
{    
    init_header();
    init_model();

    connect(this, SIGNAL(dataChanged(QModelIndex,QModelIndex)),
            this, SLOT(data_changed(QModelIndex,QModelIndex)));
}

bool user_model::get_dispatch_option(int row, user_model::DispatchRole role)
{
    return item(row, DispatchOption)->data(role).value<bool>();
}


void user_model::set_dispatch_option(int row, bool value, int role)
{        
    QStandardItem *data = item(row, role);
    if(!data){
        data = new QStandardItem;
    }
    data->setData(false, SMSId);
    data->setData(value, EmailId);
    setItem(row, DispatchOption, data);
}

void user_model::data_changed(QModelIndex top_left,
                              QModelIndex bottom_right)
{
    qDebug()<<top_left<<", "<<bottom_right;
    setData(top_left, QColor(244,100,33),
            Qt::BackgroundRole);
}

void user_model::init_header()
{    
    setHorizontalHeaderItem(DispatchOption, new QStandardItem("DispatchOption"));
    setHorizontalHeaderItem(Name, new QStandardItem("Dante must cry"));
}

void user_model::init_model()
{
    qDebug()<<SMSId<<", "<<EmailId;
    set_dispatch_option(0, true, SMSId);
    QStandardItem *data = new QStandardItem(0, Name);
    data->setData("Dante~");
}


