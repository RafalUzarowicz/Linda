#ifndef LINDA_TUPLESPACE_H
#define LINDA_TUPLESPACE_H

#include <chrono>
#include <filesystem>
#include <fcntl.h>
#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <fstream>
#include <cerrno>

#include "Constants.h"
#include "Tuple.h"
#include "Pattern.h"

void registerHandler();

namespace Linda {

    //todo should there be a destroy function? what should it do to all the waiting processes?
    // how about just removing tuplespace? other processes will get error trying to read files. There might be some problems with locked files removal
    void
    create(bool = true, const std::string& = DEFAULT_TUPLESPACE_DIR, const std::string& = DEFAULT_TUPLESPACE_NAME);

    void connect(const std::string& = DEFAULT_TUPLESPACE_PATH);

    void output(Tuple);

    Tuple input(Pattern, std::chrono::microseconds = DEFAULT_INPUT_TIMEOUT);

    Tuple read(Pattern, std::chrono::microseconds = DEFAULT_READ_TIMEOUT);
}

#endif //LINDA_TUPLESPACE_H
