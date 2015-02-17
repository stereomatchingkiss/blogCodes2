#ifndef CUSTOM_TABLE_VIEW_H
#define CUSTOM_TABLE_VIEW_H

#include <QListView>

class custom_table_view : public QListView
{
    Q_OBJECT
public:
    enum DropType
    {
        MySelf,
        Other
    };

    explicit custom_table_view(QWidget *parent = 0);
    ~custom_table_view();

    void dropEvent(QDropEvent *event);

signals:
    void my_drop_action(int source,
                        QModelIndex const &target,
                        QString const &text,
                        int drop_type);
};

#endif // CUSTOM_TABLE_VIEW_H
