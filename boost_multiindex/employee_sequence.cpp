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

template<size_t N = 1>
void print_name(employee_set const &set)
{
    if(N == 0){
        std::cout<<"------------print by sequence---------------"<<std::endl;
    }else{
        std::cout<<"------------print by set---------------"<<std::endl;
    }
    for(auto It = set.get<N>().cbegin();
        It != set.get<N>().cend(); ++It){
        std::cout<<It->name<<std::endl;
    }
}

}

void employee_sequence()
{
    employee_set set;
    auto &names = set.get<name>();
    names.emplace("Drake");
    names.emplace("Mario");
    names.emplace("Pururin");
    names.emplace("God Hand");
    names.emplace("Cocolin");
    names.emplace("Naughty Dog");
    names.emplace("RockMan");
    names.emplace("Snake");
    names.emplace("Orochi");
    names.emplace("Orochi");
    names.emplace("Orochi");
    names.emplace("Orochi");

    print_name<0>(set);
    print_name<1>(set);

    std::cout<<"------------insert by seq---------------"<<std::endl;
    auto found_by_name = names.lower_bound("Orochi");
    while(found_by_name != names.end() &&
          found_by_name->name == "Orochi"){
          auto id_it = set.project<0>(found_by_name);
          set.emplace(id_it, "Older");
          ++found_by_name;
    }

    print_name<0>(set);
    print_name<1>(set);
    auto &seq = set.get<0>();
    set.emplace(std::end(seq), "School days");
    std::cout<<"------------insert by seq---------------"<<std::endl;
    print_name<0>(set);
    print_name<1>(set);
}
