#ifndef WIDGET_MOVE_FILES_TO_OTHER_FOLDER_HPP
#define WIDGET_MOVE_FILES_TO_OTHER_FOLDER_HPP

#include <QWidget>

namespace Ui {
class widget_move_files_to_other_folder;
}

class widget_move_files_to_other_folder : public QWidget
{
    Q_OBJECT

public:
    explicit widget_move_files_to_other_folder(QWidget *parent = nullptr);
    ~widget_move_files_to_other_folder();

private slots:
    void on_pushButtonMove_clicked();

private:
    Ui::widget_move_files_to_other_folder *ui;
};

#endif // WIDGET_MOVE_FILES_TO_OTHER_FOLDER_HPP
