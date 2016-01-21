#ifndef DOWNLOAD_MANAGER_UI_HPP
#define DOWNLOAD_MANAGER_UI_HPP

#include <QMainWindow>

namespace Ui {
class download_manager_ui;
}

namespace dm{

namespace model{

class download_model;

}

class download_manager_ui : public QMainWindow
{
    Q_OBJECT

public:
    explicit download_manager_ui(QWidget *parent = 0);
    ~download_manager_ui();

    bool append(QUrl const &value, QString const &save_at,
                QString const &save_as);

private slots:
    void on_actionResume_triggered();

private:
    Ui::download_manager_ui *ui;

    model::download_model *model_;
};

}

#endif // DOWNLOAD_MANAGER_UI_HPP
