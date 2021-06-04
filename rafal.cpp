#include <iostream>

#include <Linda.h>

#define STR_BOOL(x) ((x) ? "true" : "false")

#define RAFAL
//#define GLUPOTY

struct Test{
    int x;
};

void glupieTesty(){
    Test test{};
    test.x = 0x128;
}

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

    Linda::Tuple t2(t.serialize());

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
        std::cout<<val<<'\n';
    }

    std::cout<<p<<'\n';
    std::cout<<p.path()<<'\n';

    for(auto& c : p.serialize()){
        std::cout<< static_cast<unsigned>(c) <<' ';
    }
    std::cout<<'\n';

    Linda::Pattern p2(p.serialize());

    std::cout<<p2<<'\n';
    std::cout<<p2.path()<<'\n';

    std::cout << STR_BOOL(p.check(t2)) << '\n';

//    // Wersja 1
//    Linda::TupleSpace tuplespace;
//
//    tuplespace.create();
//    tuplespace.create("tuplespace");
//    tuplespace.create("tuplespace", "./");
//
//    tuplespace.connect("./tuplespace");
//
//    tuplespace.output(t);
//
//    tuplespace.input(p);
//    tuplespace.input(p, 10000);
//
//    tuplespace.read(p2);
//    tuplespace.read(p2, 1000);

    // Wersja 2
    Linda::create();
    Linda::create("tuplespace");
    Linda::create("tuplespace", "./");

    Linda::connect("./tuplespace");

    Linda::output(t);

    Linda::input(p);
    Linda::input(p, std::chrono::milliseconds (1000));

    Linda::read(p2);
    Linda::read(p2, std::chrono::milliseconds (1000));
}

int main(){
#ifdef RAFAL
    void (*fun)() = testyRafalaXd;
#ifdef GLUPOTY
    fun = glupieTesty;
#endif
    fun();
#endif
}