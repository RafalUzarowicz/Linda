#ifndef LINDA_TUPLESPACE_H
#define LINDA_TUPLESPACE_H

#include <chrono>
#include <filesystem>

#include "Tuple.h"
#include "Pattern.h"
#include "Constants.h"
#include "State.h"
#include "TupleSpace.h"
#include "Signal.h"

namespace Linda{
    //todo should there be a destroy function? what should it do to all the waiting processes?
    void create(const std::string& = DEFAULT_TUPLESPACE_DIR, const std::string& = DEFAULT_TUPLESPACE_NAME);
    void connect(const std::string& = DEFAULT_TUPLESPACE_PATH);
    void output(Tuple);
    Tuple input(Pattern, std::chrono::milliseconds = DEFAULT_INPUT_TIMEOUT);
    Tuple read(Pattern, std::chrono::milliseconds = DEFAULT_READ_TIMEOUT);
}

#endif //LINDA_TUPLESPACE_H
