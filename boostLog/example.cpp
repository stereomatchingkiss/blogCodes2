#include <boost/log/common.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/utility/empty_deleter.hpp>
#include <boost/shared_ptr.hpp>

#include <functional>
#include <iostream>
#include <string>
#include <vector>

namespace{

bool only_warnings(const boost::log::attribute_value_set &set)
{
    return set["Severity"].extract<int>() > 0;
}

void severity_and_message(const boost::log::record_view &view,
                          boost::log::formatting_ostream &os)
{
    os << view.attribute_values()["Severity"].extract<int>() << ": " <<
                                                                view.attribute_values()["Message"].extract<std::string>();
}

}

void example_00()
{
    using namespace boost::log;
    using text_sink = sinks::asynchronous_sink<sinks::text_ostream_backend>;

    boost::shared_ptr<text_sink> sink = boost::make_shared<text_sink>();

    boost::shared_ptr<std::ostream> stream{&std::clog,
                boost::empty_deleter{}};
    sink->locked_backend()->add_stream(stream);

    core::get()->add_sink(sink);

    sources::logger lg;

    BOOST_LOG(lg) << "note";
    sink->flush();
}

void set_filter()
{
    using namespace boost::log;
    using text_sink = sinks::asynchronous_sink<sinks::text_ostream_backend>;

    auto sink = boost::make_shared<text_sink>();
    boost::shared_ptr<std::ostream> stream{&std::clog,
                boost::empty_deleter{}};
    sink->locked_backend()->add_stream(stream);
    sink->set_filter(&only_warnings);

    core::get()->add_sink(sink);

    sources::severity_logger<int> lg;

    BOOST_LOG(lg) << "note";
    BOOST_LOG_SEV(lg, 0) << "another note";
    BOOST_LOG_SEV(lg, 1) << "warning";
    sink->flush();
}

void set_format()
{
    using namespace boost::log;
    using text_sink = sinks::asynchronous_sink<sinks::text_ostream_backend>;

    auto sink = boost::make_shared<text_sink>();
    boost::shared_ptr<std::ostream> stream{&std::clog,
                boost::empty_deleter{}};
    sink->locked_backend()->add_stream(stream);
    sink->set_formatter(&severity_and_message);

    core::get()->add_sink(sink);

    sources::severity_logger<int> lg;

    BOOST_LOG_SEV(lg, 0) <<"note";
    BOOST_LOG_SEV(lg, 1) <<"warning";
    sink->flush();
}

void filter_format_lambda()
{
    using namespace boost::log;
    using text_sink = sinks::asynchronous_sink<sinks::text_ostream_backend>;

    auto sink = boost::make_shared<text_sink>();

    boost::shared_ptr<std::ostream> stream{&std::clog,
                boost::empty_deleter{}};
    sink->locked_backend()->add_stream(stream);
    sink->set_filter(expressions::attr<int>("Severity") > 0);
    sink->set_formatter(expressions::stream <<
                        expressions::attr<int>("Severity") << ": " << expressions::smessage);

    core::get()->add_sink(sink);

    sources::severity_logger<int> lg;

    BOOST_LOG_SEV(lg, 0) << "note";
    BOOST_LOG_SEV(lg, 1) << "warning";
    BOOST_LOG_SEV(lg, 2) << "error";
    sink->flush();
}

void define_keyword_for_attr()
{

}
