#include <iostream>
#include "Tuple.h"
#include "Pattern.h"

int main(){
    Tuple t;

    t.push<int>(2);
    t.push<float>(3.0f);
    t.push<std::string>("Tak");

    for(auto& val : t){
        std::cout<<val<<'\n';
    }

    std::cout<<t<<'\n';
    std::cout<<t.path()<<'\n';


    Pattern p;

//    p.add<

}