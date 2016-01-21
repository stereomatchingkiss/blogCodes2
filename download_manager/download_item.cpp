#include "download_item.hpp"

namespace dm{

namespace model{

download_item::download_item(QString const &name,
                             QString const &status,
                             QUrl const &url,
                             int_fast64_t uuid) :
    name_{name},
    status_{status},
    url_{url},
    uuid_{uuid}
{    
}

}

}
