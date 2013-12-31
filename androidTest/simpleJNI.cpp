#include "simpleJNI.hpp"

#include <QDebug>
#include <QtAndroidExtras/QAndroidJniObject>

simpleJNI::simpleJNI(QObject *parent) :
    QObject(parent)
{
}


QString simpleJNI::printHelloWorld()
{
    QAndroidJniObject str = QAndroidJniObject::callStaticObjectMethod<jstring>("adv/appBucksAdv",
                                       "getString",
                                       "(V)Ljava/lang/String;"
                                       );

    return str.toString();
}
