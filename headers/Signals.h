#ifndef LINDA_SIGNALS_H
#define LINDA_SIGNALS_H

#include <bits/types/siginfo_t.h>
#include <stdexcept>
#include <signal.h>

#include "Tuple.h"
#include "Exceptions.h"
#include "Constants.h"

namespace Linda::Signal {
    void notify(pid_t pid, int depth, int index);

    int encode(int depth, int index);

    void decode(int sigmsg, int& depth, int& index);
}

#endif //LINDA_SIGNALS_H
