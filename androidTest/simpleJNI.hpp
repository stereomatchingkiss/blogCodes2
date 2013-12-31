#ifndef SIMPLEJNI_HPP
#define SIMPLEJNI_HPP

#include <QObject>

class simpleJNI : public QObject
{
    Q_OBJECT
public:
    explicit simpleJNI(QObject *parent = nullptr);

    Q_INVOKABLE QString printHelloWorld();

};

#endif // SIMPLEJNI_HPP
