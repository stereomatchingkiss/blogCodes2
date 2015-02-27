#include "trackTableRow.hpp"

#include <QByteArray>
#include <QDataStream>
#include <QDebug>
#include <QDropEvent>
#include <QMap>
#include <QMimeData>
#include <QStandardItemModel>

trackTableRow::trackTableRow(QWidget *parent) :
    QTableView(parent)
{
    qDebug()<<__func__;
}

trackTableRow::~trackTableRow()
{

}

void trackTableRow::dropEvent(QDropEvent *event)
{
    //if ((event->proposedAction() == Qt::MoveAction)){
    auto formats = event->mimeData()->formats();
    qDebug()<<"format size : "<<formats.size();
    for(auto Data : formats){
        qDebug()<<Data;
    }

    QByteArray encoded = event->mimeData()->data(event->mimeData()->formats()[0]);
    QDataStream stream(&encoded, QIODevice::ReadOnly);
    QMap<int,QVariant> roleDataMap;
    int row, col;
    stream >> row >> col >> roleDataMap;
    qDebug()<<row<<", "<<col;

    QStandardItemModel model;
    model.dropMimeData(event->mimeData(), Qt::CopyAction, 0, 0, QModelIndex());
    //qDebug()<<"model 0,0 : "<<model.item(0, 0)->data(Qt::DisplayRole).toString();
    //model.dropMimeData(event->mimeData(), Qt::CopyAction, 0, 1, QModelIndex());
    //qDebug()<<"model 0,1 : "<<model.item(0, 1)->data(Qt::DisplayRole).toString();

    QVector<QVector<QVariant>> result;
    for(int row = 0; row != model.rowCount(); ++row){
        //qDebug()<<"drop row == "<<model.index(row, 0).row();
        result.push_back(QVector<QVariant>());
        auto &vec = result.back();
        for(int col = 0; col != model.columnCount(); ++col){
            vec.push_back(model.item(row, col)->data(Qt::DisplayRole));
        }
    }
    auto const TargetIndex = indexAt(event->pos());
    //if(TargetIndex.isValid()){
    event->acceptProposedAction();
    emit my_drop_action(row, TargetIndex, result);
    //}
    //}
}

