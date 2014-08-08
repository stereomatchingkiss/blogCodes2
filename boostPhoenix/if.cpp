#include <iostream>
#include <vector>
#include <algorithm>
#include <boost/phoenix/statement.hpp>
#include <boost/phoenix/operator.hpp>
#include <boost/phoenix/core.hpp>

void test_if()
{
    using boost::phoenix::if_;
    using boost::phoenix::arg_names::arg1;

    std::vector<int> const v{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };

    std::cout << std::dec;
    std::for_each(v.begin(), v.end(),
                  if_(arg1 > 5)
                  [
                  std::cout << arg1 << ", "
                               ]
                               );

    std::cout << std::endl;
}
