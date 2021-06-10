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
Linda::Tuple find(const Linda::Pattern& pattern, const std::string& file_path, bool remove);

namespace Linda {

    void create(bool = true, const std::string& = DEFAULT_TUPLESPACE_DIR, const std::string& = DEFAULT_TUPLESPACE_NAME);

    void connect(const std::string& = DEFAULT_TUPLESPACE_PATH);

    void disconnect();

    void output(Tuple);

    Tuple input(Pattern, std::chrono::microseconds = DEFAULT_INPUT_TIMEOUT);

    Tuple read(Pattern, std::chrono::microseconds = DEFAULT_READ_TIMEOUT);
}

#endif //LINDA_TUPLESPACE_H
