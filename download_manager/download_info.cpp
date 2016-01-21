#include "download_info.hpp"

namespace dm{

namespace net{

download_info::download_info(int_fast64_t uuid,
                             QNetworkReply *reply,
                             QString const &save_at,
                             QString const &save_as) :
    reply_(reply),
    save_at_(save_at),
    save_as_(save_as),
    uuid_{uuid}
{

}

}

}
