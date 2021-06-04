#ifndef LINDA_CONSTANTS_H
#define LINDA_CONSTANTS_H

#include <chrono>

namespace Linda{
    const static int32_t MAX_STRING_LENGTH = 64;
    const static std::string DEFAULT_TUPLESPACE_NAME = "tuplespace";
    const static std::string DEFAULT_TUPLESPACE_DIR = "./";
    const static std::chrono::milliseconds DEFAULT_TIMEOUT = std::chrono::milliseconds(10000);
    const static std::chrono::milliseconds DEFAULT_READ_TIMEOUT = DEFAULT_TIMEOUT;
    const static std::chrono::milliseconds DEFAULT_INPUT_TIMEOUT = DEFAULT_TIMEOUT;
}

#endif //LINDA_CONSTANTS_H