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

    Linda::Pattern pattern;
    pattern.add<Linda::PatternEntryType::Equal>(100);
    pattern.add<Linda::PatternEntryType::GreaterOrEqual>(4.0f);
    pattern.add<Linda::PatternEntryType::Less>("ddd");


    Linda::Tuple tuple;
    tuple.push(100).push(4.0f);

    std::cout<<STR_BOOL(pattern.check(tuple))<<'\n';



    std::cout<<Linda::Tuple()<<'\n';
    std::cout<<Linda::Pattern()<<'\n';

    Linda::Tuple t(Linda::Tuple().push(2).push(3.0f).push("Tak"));

    for(auto& val : t){
        std::cout<<val.getType()<<'\n';
    }

    std::cout<<t<<'\n';
    std::cout<<t.path()<<'\n';
    std::cout<<t.getSerializedLength()<<'\n';
    std::cout<<t.serialize().size()<<'\n';

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

    std::cout<<"all paths\n";
    for (const auto& pat : p.all_paths()){
        std::cout<<pat<<std::endl;
    }

    for(auto& c : p.serialize()){
        std::cout<< static_cast<unsigned>(c) <<' ';
    }
    std::cout<<'\n';

    std::cout<<p.getSerializedLength()<<'\n';
    Linda::Pattern p2(p.serialize());

    std::cout<<p2<<'\n';
    std::cout<<p2.path()<<'\n';
    std::cout<<p2.getSerializedLength()<<'\n';
    std::cout<<p.serialize().size()<<'\n';

    std::cout << STR_BOOL(p.check(t2)) << '\n';

    Linda::Tuple tT;

    Linda::Tuple tT2(tT.serialize());

    Linda::Pattern pP;

    Linda::Pattern pP2(pP.serialize());

    std::cout<<tT2.getSerializedLength()<<'\n';
    std::cout<<pP2.getSerializedLength()<<'\n';

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
//    tuplespace.readTuple(p2);
//    tuplespace.readTuple(p2, 1000);

//    // Wersja 2
//    Linda::create();
//    Linda::create("tuplespace");
//    Linda::create("tuplespace", "./");
//
//    Linda::connect("./tuplespace");
//
//    Linda::output(t);
//
//    Linda::input(p);
//    Linda::input(p, std::chrono::milliseconds (1000));
//
//    Linda::readTuple(p2);
//    Linda::readTuple(p2, std::chrono::milliseconds(1000));
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