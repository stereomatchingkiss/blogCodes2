#include "iterator_create.hpp"

#include <QDir>
#include <QStringList>

QDirIterator create_image_iterator(const QString &path, int recursive)
{
    return QDirIterator(path, QStringList()<<"*.jpg"<<"*.png"<<"*.bmp"<<"*.tiff"<<"*.jpeg"<<"*.webp"<<"*.gif"
                        <<"*.avi"<<"*.mp4",
                        QDir::NoDotAndDotDot | QDir::Files | QDir::Dirs,
                        static_cast<QDirIterator::IteratorFlag>(recursive));
}
