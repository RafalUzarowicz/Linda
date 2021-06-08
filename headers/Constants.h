#ifndef LINDA_CONSTANTS_H
#define LINDA_CONSTANTS_H

#include <chrono>
#include <csignal>

namespace Linda{
    const static int SIGTUPLE = SIGUSR1;
    const static int32_t MAX_STRING_LENGTH = 64;
    const static unsigned long MAX_TUPLE_SIZE = 255;
    const static unsigned long ENTRY_SIZE = MAX_TUPLE_SIZE + 2;     //pad so that there is additional char for new line
    const static int MAX_DEPTH_VALUE = 7;
    const static char EMPTY_FLAG = 'E';
    const static char BUSY_FLAG = 'B';
    const static char INPUT_FLAG = 'I';
    const static char READ_FLAG = 'R';
    const static char LIST_HEADER_SIZE = sizeof(pid_t) + 1;
    const static std::string DEFAULT_TUPLESPACE_NAME = "tuplespace";
    const static std::string DEFAULT_TUPLESPACE_DIR = "./";
    const static std::string DEFAULT_TUPLESPACE_PATH = DEFAULT_TUPLESPACE_DIR + DEFAULT_TUPLESPACE_NAME;
    // Timeouts
    const static std::chrono::milliseconds DEFAULT_TIMEOUT = std::chrono::milliseconds(10000);
    const static std::chrono::milliseconds DEFAULT_READ_TIMEOUT = DEFAULT_TIMEOUT;
    const static std::chrono::milliseconds DEFAULT_INPUT_TIMEOUT = DEFAULT_TIMEOUT;
    // Serialization
    const static ISerializable::serialization_type START_SERIALIZATION_CODE = 0x80;
    const static size_t INITAL_TUPLE_SERIALIZATION_SIZE = 2;
    const static size_t INITAL_PATTERN_SERIALIZATION_SIZE = 2;
    const static size_t SERIALIZATION_WRAP_CODES_NUMBER = 2;
    const static size_t PATTERN_TYPE_SERIALIZATION_SIZE = 1;
    const static size_t PATTERN_ENTRY_CODES_SERIALIZATION_SIZE = SERIALIZATION_WRAP_CODES_NUMBER + PATTERN_TYPE_SERIALIZATION_SIZE;
    const static size_t TUPLE_ENTRY_CODES_SERIALIZATION_SIZE = SERIALIZATION_WRAP_CODES_NUMBER;
}

#endif //LINDA_CONSTANTS_H