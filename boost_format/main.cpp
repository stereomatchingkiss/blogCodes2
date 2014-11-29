#include <boost/format.hpp>

#include <bitset>
#include <cstdint>
#include <iostream>
#include <string>
#include <type_traits>

template<typename T>
std::string int_to_ipv4(T data)
{
    static_assert(sizeof(T) * 8 == 32, "int_to_ipv4 only support"
                  " 32 bits data");
    static_assert(std::is_integral<T>::value, "T must be integral type");

    std::bitset<sizeof(T) * 8> const Bits(data);
    std::bitset<8> temp[4];
    for(size_t i = 0; i != 4; ++i){
       size_t const Offset = i * 8;
       for(size_t j = 0; j != 8; ++j){
           temp[i][j] = Bits[Offset + j];
       }
    }

    return (boost::format{"%d.%d.%d.%d"} % temp[3].to_ulong()
            % temp[2].to_ulong() %
            temp[1].to_ulong() % temp[0].to_ulong()).str();
}

int main()
{    
    std::cout<<int_to_ipv4<uint32_t>(1181772947)<<std::endl;

    std::cout<<boost::format{"%+d %d %d"} % 1 % 2 % 1<<'\n';
    return 0;
}

