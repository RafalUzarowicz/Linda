#include <iostream>

#include "Linda.h"

int main(){
    Linda::create();
    Linda::connect();
    Linda::Tuple t;
    t.push(12);
    t.push("aaa");
    t.push(10.0f);
    Linda::output(t);

    Linda::Tuple t1;
    t1.push(9);
    t1.push("zielono mi");
    t1.push(10.0f);
    Linda::output(t1);

    Linda::Pattern right;
    right.add<Linda::PatternEntryType::Equal>(12);
    right.add<Linda::PatternEntryType::Equal>("aaa");
    right.add<Linda::PatternEntryType::LessOrEqual>(18.0f);

    Linda::Pattern wrong;
    wrong.add<Linda::PatternEntryType::Equal>(12);
    wrong.add<Linda::PatternEntryType::Equal>("aaa");
    wrong.add<Linda::PatternEntryType::LessOrEqual>(3.0f);


    Linda::Tuple t13 = Linda::readTuple(wrong);
    std::cout<< Linda::readTuple(right) << std::endl;

    int depth = 7, index = 4;
    int newEncodedValue = Linda::Signal::encode(depth, index);
    printf("%d", newEncodedValue);
    Linda::Signal::decode(newEncodedValue, depth, index);
    std::cout << "\n" << depth << " " << index << std::endl;
    return 0;
}