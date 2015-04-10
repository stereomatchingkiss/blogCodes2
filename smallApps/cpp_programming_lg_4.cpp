#include "cpp_programming_lg_4.hpp"

#include <iostream>
#include <string>
#include <type_traits>

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

template<int>
struct int_exact_traits { // idea: int_exact_traits<N>::type is a type with exactly N bits
    using type = int;
};

template<>
struct int_exact_traits<8> {
    using type = char;
};

template<>
struct int_exact_traits<16> {
    using type = short;
};

}


void reference_deduction_23_5_2()
{
    std::string x {"There and back again"};
    Xref<std::string> r1 {7,"Here"}; // r1 owns a copy of str ing{"Here"}
    Xref<std::string> r2 {9,x}; // r2 just refers to x
    Xref<std::string> r3 {3,new std::string{"There"}}; // r3 owns the string{"There"}
}


void template_alias_23_6()
{
    //template<int N>
    //using int_exact = typename int_exact_traits<N>::type; // define alias for convenient notation

    //int_exact<8> a = 7; // int_exact<8> is an int with 8 bits

    std::cout<<std::is_same<int_exact_traits<8>::type, char>::value<<std::endl;
    std::cout<<std::is_same<int_exact_traits<23>::type, int>::value<<std::endl;
}
