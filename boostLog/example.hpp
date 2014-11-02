#ifndef EXAMPLE_HPP
#define EXAMPLE_HPP

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

void define_attribute();

void define_keyword_for_attr();

void example_00();

void filter_format_lambda();

void set_filter();

void set_format();

void write_to_file();

#endif // EXAMPLE_HPP
