#include <iostream>

#include "Linda.h"

int main(){
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

    Linda::Pattern right;
    right.add<Linda::PatternEntryType::Equal>(9);
    right.add<Linda::PatternEntryType::Equal>("pattern12");
    right.add<Linda::PatternEntryType::LessOrEqual>(18.0f);

    Linda::Pattern wrong;
    wrong.add<Linda::PatternEntryType::Equal>(12);
    wrong.add<Linda::PatternEntryType::Equal>("patern2");
    wrong.add<Linda::PatternEntryType::LessOrEqual>(3.0f);
    wrong.add<Linda::PatternEntryType::Any>(Linda::TupleEntryType::String);

    Linda::Pattern wrong2;
    wrong2.add<Linda::PatternEntryType::Equal>(12);
    wrong2.add<Linda::PatternEntryType::Equal>("wrong2");
    wrong2.add<Linda::PatternEntryType::LessOrEqual>(3.0f);

    Linda::Pattern right2;
    right2.add<Linda::PatternEntryType::Equal>(9);
    right2.add<Linda::PatternEntryType::Equal>("right2");
    right2.add<Linda::PatternEntryType::LessOrEqual>(18.0f);

    Linda::Pattern right3;
    right3.add<Linda::PatternEntryType::Equal>(9);
    right3.add<Linda::PatternEntryType::Equal>("right3");
    right3.add<Linda::PatternEntryType::LessOrEqual>(18.0f);

    Linda::read(wrong);
    Linda::input(right);
    Linda::read(wrong2);
    Linda::input(right2);
    Linda::input(right3);


//    Linda::Tuple t11;
//    t11.push(9);
//    t11.push("ddor t1155512");
//    t11.push(10.0f);
//    Linda::output(t11);

    return 0;
}