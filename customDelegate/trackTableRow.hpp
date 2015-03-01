#ifndef TRACKTABLEROW_H
#define TRACKTABLEROW_H

#include <QMap>
#include <QStandardItemModel>
#include <QTableView>

class trackTableRow : public QTableView
{
    Q_OBJECT
public:
    explicit trackTableRow(QWidget *parent = 0);
    ~trackTableRow();

protected:
    void dropEvent(QDropEvent *event);

signals:
    void my_drop_action(int source,
                        QModelIndex const &target,
                        QVector<QVector<QVariant>> value);

private:
    void insert_drop_data(QStandardItemModel const &drag_model,
                          QStandardItemModel *drop_model,
                          int target_row);

};

#endif // TRACKTABLEROW_H
