#ifndef BUGLOCATION_HPP
#define BUGLOCATION_HPP

#ifndef DISABLED_QT
#include <QString>
#endif

#define COMMON_DEBUG_MESSAGE  "file name : " + std::string(__FILE__) + "\nfunction name : " + __FUNCTION__ + "\ninfo : "

#ifndef DISABLED_QT
#define QCOMMON_DEBUG_MESSAGE "file name : " + QString(__FILE__) + "\nfunction name : " + __FUNCTION__ + "\ninfo : "
#endif

#endif // BUGLOCATION_HPP
