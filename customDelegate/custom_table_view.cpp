#include "custom_table_view.hpp"

#include <QByteArray>
#include <QDataStream>
#include <QDebug>
#include <QDropEvent>
#include <QMap>
#include <QMimeData>
#include <QStandardItemModel>
#include <QStringListModel>

custom_table_view::custom_table_view(QWidget *parent) :
    QListView(parent)
{    
}

custom_table_view::~custom_table_view()
{

}

QString custom_table_view::get_name() const
{
    return name_;
}

void custom_table_view::set_name(const QString &name)
{
    name_ = name;
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
        //auto const Text = roleDataMap[Qt::DisplayRole].toString();
        QStandardItemModel drag_model;
        QStringList result;
        drag_model.dropMimeData(event->mimeData(), Qt::CopyAction, 0, 0, QModelIndex());
        custom_table_view *source = qobject_cast<custom_table_view*>(event->source());
        if(source){
            qDebug()<<"source type is custom_table_view";
            qDebug()<<"source is "<<source->get_name();
            qDebug()<<"target is "<<name_;
        }
        QAbstractItemModel *target_model = model();
        auto const TargetIndex = indexAt(event->pos());
        for(int i = 0; i != drag_model.rowCount(); ++i){
            result.push_back(drag_model.data(drag_model.index(i, 0), Qt::DisplayRole).toString());
        }

        if(TargetIndex.isValid()){
            if(row >= TargetIndex.row()){
                qDebug()<<"row >= target.row";
                int target_row = TargetIndex.row();
                target_model->insertRows(TargetIndex.row(), result.size());
                for(int i = 0; i != result.size(); ++i){
                    target_model->setData(target_model->index(target_row, 0), result[i]);
                    ++target_row;
                }
            }else if(row < TargetIndex.row()){
                qDebug()<<"row < target.row";
                int target_row = TargetIndex.row() + 1;
                target_model->insertRows(target_row, result.size());
                for(int i = 0; i != result.size(); ++i){
                    target_model->setData(target_model->index(target_row, 0), result[i]);
                    ++target_row;
                }
            }
        }else{
            qDebug()<<"insert data";
            int target_row = model()->rowCount();
            target_model->insertRows(target_row, result.size());
            for(int i = 0; i != result.size(); ++i){
                target_model->setData(target_model->index(target_row, 0), result[i]);
                ++target_row;
            }
        }

        //if(TargetIndex.isValid()){
        event->acceptProposedAction();
        //emit my_drop_action(row, TargetIndex, result);
        //}
    }
}

