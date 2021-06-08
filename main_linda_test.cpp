#include <iostream>

#include "Linda.h"

int main(){
    Linda::create();
    Linda::connect();
    Linda::Tuple t;
    t.push(12);
    t.push("pattern1");
    t.push(10.0f);
    Linda::output(t);

    Linda::Tuple t1;
    t1.push(9);
    t1.push("why???");
    t1.push(10.0f);
    Linda::output(t1);

    Linda::Pattern right;
    right.add<Linda::PatternEntryType::Equal>(12);
    right.add<Linda::PatternEntryType::Equal>("pattern1");
    right.add<Linda::PatternEntryType::LessOrEqual>(18.0f);

    Linda::Pattern wrong;
    wrong.add<Linda::PatternEntryType::Equal>(12);
    wrong.add<Linda::PatternEntryType::Equal>("patern2");
    wrong.add<Linda::PatternEntryType::LessOrEqual>(3.0f);
    wrong.add<Linda::PatternEntryType::Any>(Linda::TupleEntryType::String);


    Linda::Tuple t13 = Linda::read(wrong);
    std::cout<< Linda::read(right) << std::endl;
    return 0;
}