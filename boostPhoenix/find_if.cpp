#include "find_if.hpp"

#include <vector>
#include <algorithm>
#include <iostream>
#include <iterator>
#include <boost/phoenix/core.hpp>
#include <boost/phoenix/operator.hpp>

void test_find_if()
{
    using boost::phoenix::arg_names::arg1;

    std::vector<int> const c{2, 10, 4, 5, 1, 6, 8, 3, 9, 7};
    auto it = std::find_if(c.begin(), c.end(), arg1 % 2 == 1);

    if (it != c.end())
        std::cout << *it << std::endl;
}
