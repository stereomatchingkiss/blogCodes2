#include "example_00.hpp"

#include <boost/program_options.hpp>

#include <algorithm>
#include <functional>
#include <iterator>
#include <iostream>
#include <string>
#include <stdexcept>
#include <vector>

using namespace boost::program_options;

namespace{

void on_age(float age)
{
    std::cout << "On age: " << age << '\n';
}

void to_cout(const std::vector<std::string> &v)
{
    std::copy(v.begin(), v.end(),
              std::ostream_iterator<std::string>{
                  std::cout, "\n"});
}

void iteratate_map_cpp_98(variables_map const &map)
{
    typedef variables_map::const_iterator It;
    std::cout<<"iterate map cpp98"<<std::endl;
    for(It it = map.begin(); it != map.end(); ++it){
        std::cout<<it->first<<", "<<(it->second).as<float>()<<std::endl;
    }
    std::cout<<"end\n"<<std::endl;
}

struct on_age_struct
{
    void operator()(float age) const
    {
        on_age(age);
    }

    void on_age2(float age) const
    {
        on_age(age);
    }
};

}

void basic_approach(int argc, char const *argv[])
{
    try
    {
        on_age_struct on;
        options_description desc{"Options"};
        desc.add_options()
                ("help,h", "Help screen")
                ("pi", value<float>()->default_value(3.14f), "Pi")
                ("age", value<float>()->notifier(std::bind(&on_age_struct::on_age2, &on, std::placeholders::_1)), "Age");

        variables_map vm;
        store(parse_command_line(argc, argv, desc), vm);
        notify(vm);

        iteratate_map_cpp_98(vm);

        if (vm.count("help"))
            std::cout << desc << '\n';
        else if (vm.count("age"))
            std::cout << "Age: " << vm["age"].as<float>() << '\n';
        else if (vm.count("pi"))
            std::cout << "Pi: " << vm["pi"].as<float>() << '\n';
    }
    catch (const error &ex)
    {
        std::cerr << ex.what() << '\n';
    }
}


void special_config(int argc, const char *argv[])
{
    try{
        int age = 300;

        options_description desc{"Options"};
        desc.add_options()
                ("help,h", "Help screen")
                ("pi", value<float>()->implicit_value(3.14f), "Pi")
                ("age", value<int>(&age), "Age")
                ("phone", value<std::vector<std::string>>()->multitoken()->
                 zero_tokens()->composing(), "Phone")
                ("unreg", "Unrecognized options");

        command_line_parser parser{argc, argv};
        parser.options(desc).allow_unregistered().style(
                    command_line_style::default_style |
                    command_line_style::allow_slash_for_short);
        parsed_options parsed_options = parser.run();

        variables_map vm;
        store(parsed_options, vm);
        notify(vm);

        std::cout<<"age == "<<age<<std::endl;

        if (vm.count("help"))
            std::cout << desc << '\n';
        else if (vm.count("age"))
            std::cout << "Age: " << age << '\n';
        else if (vm.count("phone"))
            to_cout(vm["phone"].as<std::vector<std::string>>());
        else if (vm.count("unreg"))
            to_cout(collect_unrecognized(parsed_options.options,
                                         exclude_positional));
        else if (vm.count("pi"))
            std::cout << "Pi: " << vm["pi"].as<float>() << '\n';
    }catch (const error &ex){
        std::cerr << ex.what() << '\n';
    }
}


void positional_options(int argc, const char *argv[])
{
    try{
        options_description desc{"Options"};
        desc.add_options()
                ("help,h", "Help screen")
                ("phone", value<std::vector<std::string>>()->
                 multitoken()->zero_tokens()->composing(), "Phone");

        positional_options_description pos_desc;
        pos_desc.add("phone", -1);

        command_line_parser parser{argc, argv};
        parser.options(desc).positional(pos_desc).allow_unregistered();
        parsed_options parsed_options = parser.run();

        variables_map vm;
        store(parsed_options, vm);
        notify(vm);

        if (vm.count("help"))
            std::cout << desc << '\n';
        else if (vm.count("phone"))
            to_cout(vm["phone"].as<std::vector<std::string>>());
    }catch (const error &ex){
        std::cerr << ex.what() << '\n';
    }
}
