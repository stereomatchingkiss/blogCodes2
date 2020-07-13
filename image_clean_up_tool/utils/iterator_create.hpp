#ifndef ITERATOR_CREATE_HPP
#define ITERATOR_CREATE_HPP

#include <QDirIterator>

QDirIterator create_image_iterator(QString const &path, int recursive = QDirIterator::NoIteratorFlags);

#endif // ITERATOR_CREATE_HPP
