#ifndef SIMPLEJNI_HPP
#define SIMPLEJNI_HPP

#include <QObject>
#include <QtAndroidExtras/QAndroidJniObject>

class simpleJNI : public QObject
{
    Q_OBJECT
public:
    explicit simpleJNI(QObject *parent = nullptr);   

private:
    QAndroidJniObject advClass_;

};

#endif // SIMPLEJNI_HPP
