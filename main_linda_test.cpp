#include <iostream>

#include "Linda.h"

int main(){
    std::cout<<(int)getpid()<<std::endl;
    Linda::create();
    Linda::connect();
//    Linda::Tuple t;
//    t.push(12);
//    t.push("pattern1-0");
//    t.push(10.0f);
//    Linda::output(t);
//
//    Linda::Tuple t1;
//    t1.push(9);
//    t1.push("pattern12");
//    t1.push(10.0f);
//    Linda::output(t1);

//    Linda::Pattern right;
//    right.add<Linda::PatternEntryType::Equal>(9);
//    right.add<Linda::PatternEntryType::Equal>("pattern12");
//    right.add<Linda::PatternEntryType::LessOrEqual>(18.0f);

//    Linda::Pattern wrong;
//    wrong.add<Linda::PatternEntryType::Equal>(9);
//    wrong.add<Linda::PatternEntryType::Equal>("pattern12");
//    wrong.add<Linda::PatternEntryType::LessOrEqual>(30.0f);
//    wrong.add<Linda::PatternEntryType::Any>(Linda::TupleEntryType::String);

    Linda::Pattern wrong2;
    wrong2.add<Linda::PatternEntryType::Equal>(10);
    wrong2.add<Linda::PatternEntryType::Equal>("pattern12");
    wrong2.add<Linda::PatternEntryType::LessOrEqual>(30.0f);
    wrong2.add<Linda::PatternEntryType::Any>(Linda::TupleEntryType::String);


//    std::cout<<Linda::read(wrong)<<std::endl;
    Linda::read(wrong2);

    Linda::Tuple t3;
    t3.push(10);
    t3.push("pattern12");
    t3.push(10.0f);
    Linda::output(t3);
//    Linda::Tuple t11;
//    t11.push(9);
//    t11.push("ddor t1155512");
//    t11.push(10.0f);
//    Linda::output(t11);

    return 0;
}