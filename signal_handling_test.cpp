//
// Created by Łukasz Pokorzyński on 08.06.2021.
//

#include <iostream>
#include <csignal>
#include "Signals.h"
#include "Linda.h"

int main() {
    pid_t pid;
    if((pid = fork()) < 0) {
        std::cout << "Could not create child process" << std::endl;
        exit(1);
    } else if(pid == 0) {
        registerHandler();
        pause();

        std::cout << "Received signal" << std::endl;
    } else {
        sleep(5);
        std::cout << "Sending signal" << std::endl;
        Linda::Signal::notify(pid, 7, 4);
    }

    return 0;
}