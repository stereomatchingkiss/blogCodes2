#include "employee.hpp"

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/identity.hpp>
#include <boost/multi_index/member.hpp>

#include <iostream>
#include <string>

using namespace boost::multi_index;

struct name{};

struct employee
{
    int         id;
    std::string name;

    employee(int id, std::string name) :
        id(id),
        name(name)
    {}
};

// define a multiply indexed set with indices by id and name
using employee_set = multi_index_container
<
    employee,
    indexed_by<
        ordered_unique<member<employee, int, &employee::id> >,
        ordered_non_unique<
            member<employee,std::string,&employee::name>
        >
    >
>;

void employee_example()
{
    employee_set set;
    set.emplace(2, "zero");
    set.emplace(3, "mario");
    set.emplace(4, "pea");
    set.emplace(5, "oliver");
    set.emplace(6, "omega");
    set.emplace(1, "rockman");
    set.emplace(0, "sonic");
    for(auto it : set.get<0>()){
        std::cout<<it.id<<" : "<<it.name<<std::endl;
    }

    //this part show you how to explicitly define the iterator type
    //in c++11, you could(and better) just use auto to deduce the
    //type of the iterator at compile time.
    using IDIter = employee_set::nth_index<0>::type::iterator;
    using NameIter = employee_set::nth_index<1>::type::iterator;

    IDIter it_id = set.get<0>().find(3);
    if(it_id != set.get<0>().end()){
        std::cout<<"found id " <<it_id->id<<" : "<<it_id->name<<std::endl;
    }

    NameIter it_name = set.get<1>().find("zero");
    if(it_name != set.get<1>().end()){
        std::cout<<"found name : "<<it_name->name<<" : "<<it_name->id<<std::endl;
    }

    std::cout<<set.size()<<std::endl;
    auto temp = std::move(set);
    std::cout<<set.size()<<std::endl;
    for(auto it : temp.get<1>()){
        std::cout<<it.name<<" : "<<it.id<<std::endl;
    }

}
