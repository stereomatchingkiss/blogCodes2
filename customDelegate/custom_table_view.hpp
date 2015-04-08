#ifndef CUSTOM_TABLE_VIEW_H
#define CUSTOM_TABLE_VIEW_H

#include <QListView>
#include <QString>

class custom_table_view : public QListView
{
    Q_OBJECT
public:    
    explicit custom_table_view(QWidget *parent = 0);
    ~custom_table_view();

    QString get_name() const;
    void set_name(QString const &name);

protected:
    void dropEvent(QDropEvent *event);

signals:
    void my_drop_action(int source,
                        QModelIndex const &target,
                        QStringList const &text);

private:
    QString name_;
};

#endif // CUSTOM_TABLE_VIEW_H
