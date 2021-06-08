//
// Created by Joanna Sokołowska on 08.06.2021.
//

#include <iostream>
#include <csignal>
#include "Linda.h"

int main() {
    pid_t pid;
    if((pid = fork()) < 0) {
        std::cout << "Could not create child process" << std::endl;
        exit(1);
    } else if(pid == 0) {
        Linda::create();
        Linda::connect();

        Linda::Tuple t;
        t.push(12);
        t.push("pattern1-0");
        t.push(10.0f);
        Linda::output(t);

        sleep(1);

        Linda::Tuple t1;
        t1.push(9);
        t1.push("pattern12");
        t1.push(10.0f);
        Linda::output(t1);

        pause();
    } else {
        Linda::create();
        Linda::connect();

        //should hang waiting for a tuple
        Linda::Pattern right;
        right.add<Linda::PatternEntryType::Equal>(9);
        right.add<Linda::PatternEntryType::Equal>("pattern12");
        right.add<Linda::PatternEntryType::LessOrEqual>(18.0f);

        std::cout<<Linda::read(right)<<std::endl;
    }
    return 0;
}