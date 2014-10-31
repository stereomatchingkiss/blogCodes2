#include "example.hpp"

#include <testFunction/testHelper.hpp>

#include <functional>
#include <iostream>
#include <string>
#include <vector>

int main()
{    
    register_test_case(std::vector<std::string>{"example_00",
                                                "set_filter",
                                                "set_format",
                                                "filter_format_lambda",
                                                "define_keyword_for_attr",
                                                "define_attribute"},
                       std::vector<std::function<void()>>{example_00,
                                                          set_filter,
                                                          set_format,
                                                          filter_format_lambda,
                                                          define_keyword_for_attr,
                                                          define_attribute});

    return 0;
}

