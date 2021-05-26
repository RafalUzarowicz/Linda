#include <iostream>
#include "Tuple.h"
#include "Pattern.h"
#include "ISerializable.h"

#define STR_BOOL(x) ((x) ? "true" : "false")

void testyRafalaXD(){
    Linda::Tuple t;
    t.push(2);
    t.push(3.0f);
    t.push("Tak\", num:3, s:\"Fajnie");

    for(auto& val : t){
        std::cout<<val.getType()<<'\n';
    }

    std::cout<<t<<'\n';
    std::cout<<t.path()<<'\n';

    for(char& c : t.serialize()){
        std::cout<< static_cast<unsigned>(c) <<' ';
    }
    std::cout<<'\n';

    Linda::Tuple t2;

    t2.deserialize(t.serialize());

    std::cout<<t2<<'\n';


//    std::vector<unsigned char> vec;
//
//    uint32_t num = 100000;
//    char c = 't';
//    std::cout<< sizeof(char)<<'\n';
//    std::cout<< &num<<'\n';
//    std::cout<< (void*)((char*)&num + 1)<<'\n';
////    for(num=0; num<1000000; num+=10000){
//        vec.clear();
//        vec.insert(vec.end(), (unsigned char*)&num, (unsigned char*)&num + sizeof(int32_t));
//        for(unsigned char& c : vec){
//            std::cout<< static_cast<unsigned>(c) <<' ';
//        }
//        std::cout<<'\n';
//        for(size_t i{}; i< sizeof(int32_t)/ sizeof(char); ++i){
//            std::cout<< static_cast<unsigned>(*((unsigned char*)((unsigned char*)&num + i*sizeof(unsigned char))))<< ' ';
////            std::cout<< ((void*)((unsigned char*)((unsigned char*)&num + i*sizeof(unsigned char))))<< ' ';
//        }
//        std::cout<<'\n';
////    }
//
//    for(auto it = vec.begin(); it != vec.end(); ++it){
//        std::cout<<static_cast<unsigned>(*it);
//    }
//    std::cout<<t.serialize().data()<<'\n';

//    Linda::Pattern p;
//
//    p.add<Linda::PatternEntryType::Equal>(2);
//    p.add<Linda::PatternEntryType::LessOrEqual>(3.0f);
//    p.add<Linda::PatternEntryType::Equal>("Tak");
//    p.add<Linda::PatternEntryType::Any>(Linda::TupleEntryType::Int);
//    p.add<Linda::PatternEntryType::Any>(Linda::TupleEntryType::Float);
//    p.add<Linda::PatternEntryType::Any>(Linda::TupleEntryType::String);
//
//    for(auto& val : p){
//        std::cout<<val.to_string()<<'\n';
//    }
//
//    std::cout<<p<<'\n';
//
//    std::cout << STR_BOOL(p.check(t)) << '\n';
}

int main(){
    void (*fun)() = testyRafalaXD;

    fun();
}