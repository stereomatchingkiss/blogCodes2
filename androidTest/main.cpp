#include <QQmlContext>
#include <QQmlEngine>
#include <QtGui/QGuiApplication>
//#include <QQuickView>

#include "qtquick2applicationviewer/qtquick2applicationviewer.h"
#include "simpleJNI.hpp"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QtQuick2ApplicationViewer viewer;
    simpleJNI *jni = new simpleJNI(&viewer);
    viewer.engine()->rootContext()->setContextProperty(QLatin1String("simpleJNI"),
                                                     jni);
    viewer.setSource(QStringLiteral("qrc:///qml/androidTest/main.qml"));
    //view.setSource(QUrl(QStringLiteral("qrc:/qml/main.qml")));
    viewer.showExpanded();

    //QQuickView view;
    //view.setSource(QUrl("/Users/Qt/program/experiment_apps_and_libs/androidTest/qml/androidTest/main.qml"));
    //view.show();

    return app.exec();
}
