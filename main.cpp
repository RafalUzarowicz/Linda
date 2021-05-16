#include <iostream>
#include "Tuple.h"
#include "Pattern.h"

#define STRBOOL(x) (x == true ? "true" : "false")

int main(){
    Tuple t;

    t.push(2);
    t.push(3.0f);
    t.push("Tak");

    for(auto& val : t){
        std::cout<<val.getType()<<'\n';
    }

    std::cout<<t<<'\n';
    std::cout<<t.path()<<'\n';

    Pattern p;

    p.add<PatternEntryType::Equal>(2);



    std::cout<<STRBOOL(p.check(t))<<'\n';
}