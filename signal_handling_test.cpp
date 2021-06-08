//
// Created by Łukasz Pokorzyński on 08.06.2021.
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
        struct sigaction act = {};
        act.sa_sigaction = Linda::sighandler;
        act.sa_flags = SA_SIGINFO;
        sigaction(Linda::SIGTUPLE, &act, nullptr);

        pause();
    } else {
        sleep(5);
        std::cout << "Sending signal" << std::endl;
        Linda::Signal::notify(pid, 7, 4);
    }

    return 0;
}