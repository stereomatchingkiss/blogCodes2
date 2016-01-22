#ifndef DOWNLOAD_INFO_H
#define DOWNLOAD_INFO_H

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/identity.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/random_access_index.hpp>

#include <QFile>
#include <QNetworkReply>
#include <QString>

#include <cstdint>
#include <memory>

namespace dm{

namespace net{

struct download_info
{
    download_info() = default;
    download_info(int_fast64_t uuid,
                  QNetworkReply *reply,
                  QString const &save_at,
                  QString const &save_as);

    QString error_;
    //QFile is noncopyable, this is why
    //shared ptr is needed
    std::shared_ptr<QFile> file_;
    QNetworkReply *reply_ = nullptr;
    QString save_at_;
    QString save_as_;
    QUrl url_;
    int_fast64_t uuid_ = 0;
};

struct net_reply{};
struct uid{};

using download_info_index = boost::multi_index::multi_index_container
<
    download_info,
    boost::multi_index::indexed_by<
        //The reply_ should be unique from logical view,
        //I declare it as nonunique to make multiple
        //download task easier to manage
        boost::multi_index::ordered_non_unique<
            boost::multi_index::tag<net_reply>,
            boost::multi_index::member<download_info,QNetworkReply*,&download_info::reply_>
        >,
        boost::multi_index::ordered_unique<
            boost::multi_index::tag<uid>,
            boost::multi_index::member<download_info,int_fast64_t,&download_info::uuid_>
        >
    >
>;

}

}

#endif // DOWNLOAD_INFO_H
