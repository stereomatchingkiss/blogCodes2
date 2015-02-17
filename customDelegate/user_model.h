#ifndef USER_MODEL_H
#define USER_MODEL_H

#include <QString>
#include <QStandardItemModel>

class user_model : public QStandardItemModel
{
    Q_OBJECT
public:
    enum DispatchUser {        
        DispatchOption,
        Name
    };

    enum DispatchRole{
        EmailId = Qt::UserRole + 1,
        SMSId
    };

    explicit user_model(QObject *obj = 0);    

    bool get_dispatch_option(int row, DispatchRole role);

    void set_dispatch_option(int row, bool value, int role);

private slots:
    void data_changed(QModelIndex top_left, QModelIndex bottom_right);

private:        
    void init_header();    
    void init_model();
};

#endif // USER_MODEL_H

