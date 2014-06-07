#include "moveLibsAndDLL.hpp"

#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QString>

#include <iostream>

void move_libs_and_dll(QStringList const &filter)
{
    auto const CurrentPath = QDir::currentPath();
    QDirIterator directories(CurrentPath, QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
    QStringList dirs;
    while(directories.hasNext()){
        directories.next();
        dirs.push_back(directories.filePath());
    }

    QDir dir(CurrentPath);
    dir.mkdir("libsFolder");
    for(auto const &Str : dirs){
        QDir directory(Str);
        auto const Fileinfos = directory.entryList(filter,
                                                   QDir::Files);
        for(auto const &File : Fileinfos){
            directory.rename(File, CurrentPath + "/libsFolder/" + File);
        }
    }
}
