#include "custom_table_view.h"

#include <QByteArray>
#include <QDataStream>
#include <QDebug>
#include <QDropEvent>
#include <QMap>
#include <QMimeData>

custom_table_view::custom_table_view(QWidget *parent) :
    QListView(parent)
{
    qDebug()<<__func__;
}

custom_table_view::~custom_table_view()
{

}

void custom_table_view::dropEvent(QDropEvent *event)
{
    if ((event->proposedAction() == Qt::MoveAction)){
        QByteArray encoded = event->mimeData()->data(event->mimeData()->formats()[0]);
        QDataStream stream(&encoded, QIODevice::ReadOnly);
        QMap<int,  QVariant> roleDataMap;
        int row, col;
        stream >> row >> col >> roleDataMap;
        qDebug()<<row<<", "<<col;
        auto const Text = roleDataMap[Qt::DisplayRole].toString();
        qDebug()<<roleDataMap[Qt::DisplayRole].toString();

        auto const TargetIndex = indexAt(event->pos());
        if(TargetIndex.isValid()){
            qDebug()<<"drag to : "<<TargetIndex.row()<<", "<<TargetIndex.column();
            event->acceptProposedAction();
            emit my_drop_action(row, TargetIndex, Text, MySelf);
        }
    }
}

