#include "example00.hpp"

#include <testFunction/testHelper.hpp>

#include <functional>
#include <iostream>
#include <string>
#include <vector>

int main(int argc, char const* argv[])
{    
    std::vector<std::string> const Names{
        "example_00",
        "example_01",
        "mapped_region_00",
        "mapped_memory_read_and_write_00",
        "managed_shared_memory_00",
        "managed_shared_memory_01",
        "boost_interprocess_string",
        "atomic_func_00"};

    std::vector<std::function<void()>> funcs{
                                       std::bind(example_00, argc, argv),
                                       example_01,
                                       mapped_region_00,
                                       mapped_memory_read_and_write_00,
                                       managed_shared_memory_00,
                                       managed_shared_memory_01,
                                       boost_interprocess_string,
                                       atomic_func_00};

    register_test_case(Names, funcs);

    return 0;
}

