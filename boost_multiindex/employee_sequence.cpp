#include "employee_sequence.hpp"

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/identity.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/sequenced_index.hpp>

#include <iostream>
#include <string>

namespace{

using namespace boost::multi_index;

struct name{};

struct employee
{
    std::string name;

    employee(std::string name) :
        name(name)
    {}
};

// define a multiply indexed set with indices by id and name
using employee_set = multi_index_container
<
    employee,
    indexed_by<
        sequenced<>,
        ordered_non_unique<
            tag<name>,
            member<employee,std::string,&employee::name>
        >
    >
>;

void print_name(employee_set const &set)
{
    for(auto NameIt = set.get<name>().cbegin();
        NameIt != set.get<name>().cend(); ++NameIt){
        std::cout<<NameIt->name<<std::endl;
    }
}

}

void employee_sequence()
{
    employee_set set;
    set.get<name>().emplace("Drake");
    set.get<name>().emplace("Mario");
    set.get<name>().emplace("Pururin");
    set.get<name>().emplace("God Hand");
    set.get<name>().emplace("Cocolin");
    set.get<name>().emplace("Naughty Dog");
    set.get<name>().emplace("RockMan");
    set.get<name>().emplace("Snake");
    set.get<name>().emplace("Orochi");
    set.get<name>().emplace("Orochi");
    set.get<name>().emplace("Orochi");
    set.get<name>().emplace("Orochi");

    std::cout<<"------------insert by set---------------"<<std::endl;
    for(auto NameIt = set.cbegin();
        NameIt != set.cend(); ++NameIt){
        std::cout<<NameIt->name<<std::endl;
    }

    std::cout<<"------------insert by seq---------------"<<std::endl;
    auto found_by_name = set.get<1>().lower_bound("Orochi");
    while(found_by_name != set.get<name>().end() &&
          found_by_name->name == "Orochi"){
          auto id_it = set.project<0>(found_by_name);
          set.emplace(id_it, "older");
          ++found_by_name;
    }

    print_name(set);
    set.get<name>().emplace("School days");

    std::cout<<"------------insert by seq---------------"<<std::endl;
    print_name(set);
}
