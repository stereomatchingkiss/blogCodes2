#include "download_item.hpp"

namespace dm{

namespace model{

download_item::download_item(QString const &name,
                             QString const &status,
                             int_fast64_t uuid) :
    name_{name},
    status_{status},
    uuid_{uuid}
{    
}

}

}
