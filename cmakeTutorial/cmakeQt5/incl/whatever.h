#include <QObject>
#include <QDebug>

class whatever : public QObject
{
    Q_OBJECT

public:
    whatever();

public slots:
    void print();
};
