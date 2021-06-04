#ifndef LINDA_TUPLESPACE_H
#define LINDA_TUPLESPACE_H

#include <chrono>

#include "Tuple.h"
#include "Pattern.h"
#include "Constants.h"
#include "State.h"

namespace Linda{
    void create(const std::string& = DEFAULT_TUPLESPACE_NAME, const std::string& = DEFAULT_TUPLESPACE_DIR);
    void connect(const std::string&);
    void output(Tuple);
    Tuple input(Pattern, std::chrono::milliseconds = DEFAULT_INPUT_TIMEOUT);
    Tuple read(Pattern, std::chrono::milliseconds = DEFAULT_READ_TIMEOUT);
}

#endif //LINDA_TUPLESPACE_H
