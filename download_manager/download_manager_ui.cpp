#include "download_manager_ui.hpp"
#include "ui_download_manager_ui.h"

#include "download_model.hpp"

#include <QTableView>

namespace dm{

download_manager_ui::download_manager_ui(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::download_manager_ui)
{
    ui->setupUi(this);

    model_ = new model::download_model(this);
    ui->tableViewDownload->setModel(model_);
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
