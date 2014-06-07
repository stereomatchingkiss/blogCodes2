#ifndef RUNSOMETHING_HPP
#define RUNSOMETHING_HPP

#include <QObject>

class runSomething : public QObject
{
    Q_OBJECT
public:
    runSomething(QObject *parent = nullptr);

    void print_value(int value);

private slots:
    void printValue(int value);

signals:
    void valueChanged(int newValue);

};

#endif // RUNSOMETHING_HPP
