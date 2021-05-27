#include <iostream>
#include "Tuple.h"
#include "Pattern.h"

#define STR_BOOL(x) ((x) ? "true" : "false")

void testyRafalaXd(){

    Linda::Tuple t(Linda::Tuple().push(2).push(3.0f).push("Tak"));

    for(auto& val : t){
        std::cout<<val.getType()<<'\n';
    }

    std::cout<<t<<'\n';
    std::cout<<t.path()<<'\n';

    for(auto& c : t.serialize()){
        std::cout<< static_cast<unsigned>(c) <<' ';
    }
    std::cout<<'\n';

    Linda::Tuple t2;

    t2.deserialize(t.serialize());

    Linda::Tuple t3(t.serialize());

    std::cout<<t2<<'\n';
    std::cout<<t3<<'\n';

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
    std::cout<<p.path()<<'\n';

    for(auto& c : p.serialize()){
        std::cout<< static_cast<unsigned>(c) <<' ';
    }
    std::cout<<'\n';

    Linda::Pattern p2;

    p2.deserialize(p.serialize());

    std::cout<<p2<<'\n';

    std::cout << STR_BOOL(p.check(t2)) << '\n';
}

int main(){
    void (*fun)() = testyRafalaXd;

    fun();
}