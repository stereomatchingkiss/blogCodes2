#include "advanceExample.hpp"

#include <boost/log/attributes.hpp>
#include <boost/log/common.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/utility/empty_deleter.hpp>
#include <boost/shared_ptr.hpp>

#include <fstream>
#include <functional>
#include <iostream>
#include <string>
#include <vector>

namespace{

enum severity_level
{
    normal,
    notification,
    warning,
    error,
    critical
};

}

void source_specific_attr()
{
    using namespace boost::log;

    sources::severity_logger<severity_level> log;

    BOOST_LOG_SEV(log, normal)<<"all green";
    BOOST_LOG_SEV(log, warning)<<"something bad is going on, but I can handle it";
    BOOST_LOG_SEV(log, critical)<<"shut me down";
}
