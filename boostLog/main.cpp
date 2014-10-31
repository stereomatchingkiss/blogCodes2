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
                                                "filter_format_lambda"},
                       std::vector<std::function<void()>>{example_00,
                                                          set_filter,
                                                          set_format,
                                                          filter_format_lambda});

    return 0;
}

