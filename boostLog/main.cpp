#include "addAttr.hpp"
#include "advanceExample.hpp"
#include "example.hpp"
#include "fmtStream.hpp"
//#include "example_00.hpp"

#include <testFunction/testHelper.hpp>

#include <functional>
#include <iostream>
#include <string>
#include <vector>

int main()
{    
    using Type = std::vector<std::pair<std::string,
    std::function<void()>>>;

    register_test_case(Type{
                           {"example_00", example_00},
                           {"set_filter", set_filter},
                           {"set_format", set_format},
                           {"filter_format_lambda", filter_format_lambda},
                           {"define_keyword_for_attr", define_keyword_for_attr},
                           {"define_attribute", define_attribute},
                           {"write_to_file", write_to_file},
                           {"source_specific_attr", source_specific_attr},
                           {"add_attr", add_attr},
                           {"fmt_stream", fmt_stream}
                       });
    return 0;
}

