#include <QApplication>

#include "histogramBackProjection.hpp"
#include "histBackProjectUI.hpp"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //histogramBackProjection histProj;
    //histProj.show();
    histBackProjectUI histProj;
    histProj.show();
    
    return a.exec();
}
