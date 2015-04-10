#include "cpp_programming_lg_4.h"

#include <iostream>
#include <string>

namespace{

template<typename T>
class Xref {
public:
    Xref(int i, T* p) // store a pointer: Xref is the owner
        :index{i}, elem{p}, owned{true}
    {
        std::cout<<"store a pointer: Xref is the owner"<<std::endl;
    }
    Xref(int i, T& r) // store a pointer to r, owned by someone else
        :index{i}, elem{&r}, owned{false}
    {
        std::cout<<"store a pointer to r, owned by someone else"<<std::endl;
    }
    Xref(int i, T&& r) // move r into Xref, Xref is the owner
        :index{i}, elem{new T{move(r)}}, owned{true}
    {
        std::cout<<"move r into Xref, Xref is the owner"<<std::endl;
    }
    ~Xref()
    {
        if(owned) delete elem;
    }

private:
    int index;
    T* elem;
    bool owned;
};

}


void reference_deduction_23_5_2()
{
    std::string x {"There and back again"};
    Xref<std::string> r1 {7,"Here"}; // r1 owns a copy of str ing{"Here"}
    Xref<std::string> r2 {9,x}; // r2 just refers to x
    Xref<std::string> r3 {3,new std::string{"There"}}; // r3 owns the string{"There"}
}
