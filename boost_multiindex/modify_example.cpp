#include "modify_example.hpp"

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/identity.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/sequenced_index.hpp>

#include <iostream>
#include <string>

using namespace boost::multi_index;

namespace{

struct name{};
struct email{};

struct employee
{
    std::string email_;
    std::string name_;

    employee(std::string name, std::string email) :
        email_(email),
        name_(name)
    {}
};

using DispatchUsersTable =
multi_index_container
<
  employee,
  indexed_by
  <
    ordered_unique
    <
      tag<name>,
      member<employee, std::string, &employee::name_>
    >,
    ordered_unique
    <
      tag<email>,
      member<employee, std::string, &employee::email_>
    >
  >//end indexed_by
>;

}

void order_after_modify()
{
    DispatchUsersTable table;
    table.insert(employee{"jk", "http:cat"});
    table.insert(employee{"otaku", "http:joker"});
    table.insert(employee{"cba", "http:amota"});

    //auto will make codes much easier to read
    //I type the type explicitly because I need to
    //maintain the codes created by c++98
    using Table = DispatchUsersTable::index<name>::type;
    using Iterator = Table::iterator;

    for(Iterator it = table.get<name>().begin();
        it != table.get<name>().end(); ++it){
        std::cout<<it->name_<<", "<<it->email_<<std::endl;
    }

    Table &name_index = table.get<name>();
    Iterator it = name_index.find("jk");
    if(it != name_index.end()){
        name_index.modify(it, [&](employee &e){ e.name_ = "buddy"; });
    }else{
        std::cout<<"cannot find jk"<<std::endl;
    }

    std::cout<<std::endl;
    for(Iterator it = table.get<name>().begin();
        it != table.get<name>().end(); ++it){
        std::cout<<it->name_<<", "<<it->email_<<std::endl;
    }
}
