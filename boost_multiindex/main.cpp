#include <iostream>
#include <string>

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/identity.hpp>
#include <boost/multi_index/member.hpp>

using namespace boost::multi_index;

struct name{};

struct employee
{
    int         id;
    std::string name;

    employee(int id,const std::string& name) :
        id(id),
        name(name)
    {}

    bool operator<(const employee& e)const{return id < e.id;}
};

// define a multiply indexed set with indices by id and name
using employee_set = multi_index_container
<
    employee,
    indexed_by<
        ordered_unique<identity<employee>>,
    ordered_non_unique<
        tag<name>,
        member<employee,std::string,&employee::name>>
    >
>;

int main()
{    
    employee_set set;
    set.insert(employee(3, "ss"));
    set.insert(employee(4, "aa"));
    set.insert(employee(5, "hh"));
    for(auto it : set.get<name>()){
        std::cout<<it.id<<" : "<<it.name<<std::endl;
    }

    return 0;
}

