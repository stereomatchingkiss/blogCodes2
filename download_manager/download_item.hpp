#ifndef DOWNLOAD_ITEM_H
#define DOWNLOAD_ITEM_H

#include "global_variable.hpp"

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/identity.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/random_access_index.hpp>

#include <QString>
#include <QUrl>

#include <cstdint>

namespace dm{

namespace model{

struct random{};
struct uid{};

struct download_item
{    
    enum class tag{
        name,
        percent,
        size,
        status
    };

    download_item() = default;
    download_item(QString const &name,
                  QString const &status,
                  QUrl const &url,
                  int_fast64_t uuid);

    QString name_;
    QString percent_ = "?";
    size_t size_ = 0;
    QString status_ = global::waiting;
    QUrl url_;
    int_fast64_t uuid_ = 0;
};

using download_index = boost::multi_index::multi_index_container
<
    download_item,
    boost::multi_index::indexed_by<
        boost::multi_index::random_access<boost::multi_index::tag<random>>,
        boost::multi_index::ordered_unique<
            boost::multi_index::tag<uid>,
            boost::multi_index::member<download_item,int_fast64_t,&download_item::uuid_>
        >
    >
>;

}

}

#endif // DOWNLOAD_ITEM_H
