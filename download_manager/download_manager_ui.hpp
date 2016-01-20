#ifndef DOWNLOAD_MANAGER_UI_HPP
#define DOWNLOAD_MANAGER_UI_HPP

#include <QMainWindow>

namespace Ui {
class download_manager_ui;
}

class download_manager_ui : public QMainWindow
{
    Q_OBJECT

public:
    explicit download_manager_ui(QWidget *parent = 0);
    ~download_manager_ui();

private:
    Ui::download_manager_ui *ui;
};

#endif // DOWNLOAD_MANAGER_UI_HPP
