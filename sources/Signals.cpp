#include <string>
#include <csignal>
#include "Signals.h"

#define LAST(k, n) ((k) & ((1<<(n))-1))
#define MID(k, m, n) LAST((k)>>(m),((n)-(m)))

void Linda::Signal::notify(pid_t pid, int depth, int index) {
    union sigval sigvalMsg = {};
    sigvalMsg.sival_int = encode(depth, index);
    sigqueue(pid, SIGTUPLE, sigvalMsg);
}

int Linda::Signal::encode(int depth, int index) {
    if (depth > MAX_DEPTH_VALUE) {
        throw Linda::Exception::Signal::EncodingException("Too big depth value: " + std::to_string(depth));
    }

    int encodedValue = 0;
    encodedValue += index;
    encodedValue = encodedValue << 3;
    encodedValue = encodedValue | depth;

    return encodedValue;
}

void Linda::Signal::decode(int sigmsg, int& depth, int& index) {
    depth = MID(sigmsg, 0, 3);
    index = MID(sigmsg, 3, 32);
}