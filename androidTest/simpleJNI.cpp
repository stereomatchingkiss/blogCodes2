#include "simpleJNI.hpp"

#include <QtAndroidExtras/QAndroidJniObject>

simpleJNI::simpleJNI(QObject *parent) :
    QObject(parent)
{
}


QString simpleJNI::printHelloWorld()
{
    QAndroidJniObject str = QAndroidJniObject::callStaticObjectMethod("adv/appBucksAdv",
                                       "printHelloWorld",
                                       "(I)Ljava/lang/String;",
                                       23);

    return str.toString();
}
str.toString();
}
