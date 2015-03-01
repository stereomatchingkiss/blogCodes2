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

void trackTableRow::insert_drop_data(QStandardItemModel const &drag_model,
                                     QStandardItemModel *drop_model,
                                     int target_row)
{
    drop_model->insertRows(target_row, drag_model.rowCount());
    for(int i = 0; i != drag_model.rowCount(); ++i){
        //auto const &Vec = result[i];
        drop_model->setData(drop_model->index(target_row, 0),
                            drag_model.data(drag_model.index(i, 0)).toString());
        drop_model->setData(drop_model->index(target_row, 1),
                            drag_model.data(drag_model.index(i, 1)).toInt());
        ++target_row;
    }
}

void trackTableRow::dropEvent(QDropEvent *event)
{
    if ((event->proposedAction() == Qt::MoveAction)){
        QByteArray encoded = event->mimeData()->data(event->mimeData()->formats()[0]);
        QDataStream stream(&encoded, QIODevice::ReadOnly);
        int source_row;
        stream >> source_row;

        QStandardItemModel drag_model;
        drag_model.dropMimeData(event->mimeData(), Qt::CopyAction, 0, 0, QModelIndex());

        QModelIndex const TargetIndex = indexAt(event->pos());
        QStandardItemModel *drop_model = qobject_cast<QStandardItemModel*>(model());
        if(TargetIndex.isValid() && drop_model){
            if(source_row >= TargetIndex.row()){
                insert_drop_data(drag_model, drop_model, TargetIndex.row());
            }else if(source_row < TargetIndex.row()){
                insert_drop_data(drag_model, drop_model, TargetIndex.row() + 1);
            }
        }else{
            insert_drop_data(drag_model, drop_model, model()->rowCount());
        }
        event->acceptProposedAction();
    }
}

