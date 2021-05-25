#include <iostream>
#include "Tuple.h"
#include "Pattern.h"

#define STR_BOOL(x) ((x) ? "true" : "false")

void testyRafalaXD(){
    Linda::Tuple t;
    t.push(2);
    t.push(3.0f);
    t.push("Tak");

    for(auto& val : t){
        std::cout<<val.getType()<<'\n';
    }

    std::cout<<t<<'\n';
    std::cout<<t.path()<<'\n';

    Linda::Pattern p;

    p.add<Linda::PatternEntryType::Equal>(2);
    p.add<Linda::PatternEntryType::LessOrEqual>(3.0f);
    p.add<Linda::PatternEntryType::Equal>("Tak");
    p.add<Linda::PatternEntryType::Any>(Linda::TupleEntryType::Int);
    p.add<Linda::PatternEntryType::Any>(Linda::TupleEntryType::Float);
    p.add<Linda::PatternEntryType::Any>(Linda::TupleEntryType::String);

    for(auto& val : p){
        std::cout<<val.to_string()<<'\n';
    }

    std::cout<<p<<'\n';

    std::cout << STR_BOOL(p.check(t)) << '\n';
}

int main(){
    void (*fun)() = testyRafalaXD;

    fun();
}