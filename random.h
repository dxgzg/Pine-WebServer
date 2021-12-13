#pragma once
#include <random>
#include <string>

const char seed[64] = {
    '0','1','2','3','4','5','6','7','8','9',
    'A','B','C','D','E','F','G','H','I','J',
    'K','L','M','N','O','P','Q','R','S','T',
    'U','V','W','X','Y','Z','a','b','c','d',
    'e','f','g','h','i','j','k','l','m','n',
    'o','p','q','r','s','t','u','v','w','x',
    'y','z','\0','\0'
};
std::string getName(int n){
    static std::random_device rd;  // 将用于为随机数引擎获得种子
    static std::mt19937 gen(rd()); // 以播种标准 mersenne_twister_engine
    static std::uniform_int_distribution<> dis(0, 62);
    
    std::string name = "";
    for (int i = 0; i < n; ++i){
        // 用 dis 变换 gen 所生成的随机 unsigned int 到 [0, 62] 中的 int
        int val = dis(gen);
        name += seed[val];
    }
    return name;
}
