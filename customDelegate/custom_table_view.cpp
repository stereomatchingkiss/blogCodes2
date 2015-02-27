#include "custom_table_view.hpp"

#include <QByteArray>
#include <QDataStream>
#include <QDebug>
#include <QDropEvent>
#include <QMap>
#include <QMimeData>
#include <QStandardItemModel>

custom_table_view::custom_table_view(QWidget *parent) :
    QListView(parent)
{    
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
        QStandardItemModel model;
        QStringList result;
        model.dropMimeData(event->mimeData(), Qt::CopyAction, 0, 0, QModelIndex());
        for(int i = 0; i != model.rowCount(); ++i){
            result.push_back(model.data(model.index(i, 0), Qt::DisplayRole).toString());
        }

        auto const TargetIndex = indexAt(event->pos());
        //if(TargetIndex.isValid()){
            event->acceptProposedAction();
            emit my_drop_action(row, TargetIndex, result);
        //}
    }
}

