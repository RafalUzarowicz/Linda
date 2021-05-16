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
    p.add<PatternEntryType::LessOrEqual>(3.0f);
    p.add<PatternEntryType::Equal>("Tak");
    p.add<PatternEntryType::Any>(TupleEntryType::Int);
    p.add<PatternEntryType::Any>(TupleEntryType::Float);
    p.add<PatternEntryType::Any>(TupleEntryType::String);

    for(auto& val : p){
        std::cout<<val.to_string()<<'\n';
    }

    std::cout<<p<<'\n';

    std::cout<<STRBOOL(p.check(t))<<'\n';
}