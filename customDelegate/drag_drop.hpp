#ifndef DRAG_DROP_HPP
#define DRAG_DROP_HPP

#include "custom_table_view.hpp"

#include <QDialog>
#include <QStringListModel>

namespace Ui {
class drag_drop;
}

class QModelIndex;

class drag_drop : public QDialog
{
    Q_OBJECT

public:
    explicit drag_drop(QWidget *parent = 0);
    ~drag_drop();

private slots:
    void on_pushButtonPrint_clicked();

    void sort_model(QModelIndex, QModelIndex);

    void left_drop_action(int row,
                          QModelIndex const &target,
                          QString const &text,
                          int drop_type);

private:
    void drop_action_impl(int row,
                          QModelIndex const &target,
                          QString const &text,
                          QStringListModel &model);

    QStringListModel left_model_;
    QStringListModel right_model_;

    Ui::drag_drop *ui;
};

#endif // DRAG_DROP_HPP
