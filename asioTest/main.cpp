#include "getLocalHostAddr.hpp"
#include "noWork00.hpp"
#include "dispatch00.hpp"
#include "post00.hpp"
#include "timer00.hpp"

#include <testFunction/testHelper.hpp>

#include <functional>
#include <iostream>
#include <string>
#include <vector>

int main()
{
    std::vector<std::string> const Names{"dispatch example 00", "no work 00",
                                         "post example 00", "timer example 00",
                                         "getLocalHostAddr"};

    std::vector<std::function<void()>> func{
                                       dispatch_example_00, no_work_00,
                                       post_example_00, timer_00,
                                       get_local_host_address};

    register_test_case(Names, func);

    return 0;
}
