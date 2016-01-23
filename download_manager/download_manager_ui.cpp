#include "download_manager_ui.hpp"
#include "ui_download_manager_ui.h"

#include "download_model.hpp"
#include "global_variable.hpp"

#include <QDebug>
#include <QTableView>

namespace dm{

download_manager_ui::download_manager_ui(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::download_manager_ui)
{
    ui->setupUi(this);

    model_ = new model::download_model(this);
    ui->tableViewDownload->setModel(model_);
    ui->tableViewDownload->setShowGrid(false);
    ui->tableViewDownload->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableViewDownload->setSelectionMode(QAbstractItemView::ExtendedSelection);
}

download_manager_ui::~download_manager_ui()
{
    delete ui;
}

bool download_manager_ui::append(const QUrl &value,
                                 const QString &save_at,
                                 const QString &save_as)
{
    return model_->append(value, save_at, save_as);
}

}

void dm::download_manager_ui::on_actionResume_triggered()
{
    auto *select_model = ui->tableViewDownload->selectionModel();
    qDebug()<<__func__<<" select model size "<<select_model->selectedRows().size();
    for(auto const &select_index : select_model->selectedRows()){
        qDebug()<<__func__<<" start download";
        model_->start_download(select_index.row());
    }
}

void dm::download_manager_ui::
on_tableViewDownload_clicked(const QModelIndex &index)
{
    using tag = model::download_item::tag ;

    qDebug()<<__func__<<" : "<<index;

    auto const status =
            model_->data(model_->index(index.row(),
                                       static_cast<int>(tag::status)),
                         Qt::DisplayRole).toString();
    bool const can_resume = status == global::waiting ||
            status == global::unknown ||
            status == global::error;
    ui->actionResume->setEnabled(can_resume);
}
