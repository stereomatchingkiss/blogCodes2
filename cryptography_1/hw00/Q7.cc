#include <bitset>
#include <iomanip>
#include <iostream>
#include <string>

void question_07()
{
    //std::string const M2 = "u";
    //std::string const C2 = "94";
    std::string const M1 = "awn";
    std::string const M2 = "usk";
    std::string const C2 = "e53f36";
    for(size_t i = 0; i != M2.size(); ++i){
        std::bitset<8> const BitsM1 = M1[i];
        std::bitset<8> const BitsM2 = M2[i];
        std::bitset<8> const BitsC2(std::stoi(C2.substr(i * 2, 2), nullptr, 16));
        //std::cout<<std::hex<<BitsM1.to_ulong()<<", "<<BitsM2.to_ulong();
        //std::cout<<", "<<std::hex<<BitsC2.to_ulong()<<std::endl;
        std::bitset<8> answer = BitsM1;
        answer ^= BitsM2;
        answer ^= BitsC2;
        std::cout<<std::hex<<answer.to_ulong()<<std::endl;
    }
}
