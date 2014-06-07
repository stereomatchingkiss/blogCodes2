#include "runSomething.hpp"

#include <boost/thread.hpp>

#include <iostream>

runSomething::runSomething(QObject *parent) : QObject(parent)
{
    connect(this, SIGNAL(valueChanged(int)), this, SLOT(printValue(int)));
}

void runSomething::print_value(int value)
{
    std::cout<<"print value : "<<std::endl;
    emit valueChanged(value);
}

void runSomething::printValue(int value)
{
    std::cout<<"print value class : "<<boost::this_thread::get_id()<<std::endl;
    std::cout<<value<<std::endl;
}
