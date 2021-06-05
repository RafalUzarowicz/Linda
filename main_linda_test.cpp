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
    Linda::output(t1);
    return 0;
}