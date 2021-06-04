#ifndef LINDA_STATE_H
#define LINDA_STATE_H

#include "Constants.h"

namespace Linda{
    class State{
    public:
        State(State&) = delete;
        void operator=(const State&) = delete;
    private:
        State() : tupleSpaceDirName(DEFAULT_TUPLESPACE_NAME), tupleSpacePath(DEFAULT_TUPLESPACE_DIR) {

        }

        static State instance;

        std::string tupleSpaceDirName{};
        std::string tupleSpacePath{};

        friend void create(const std::string&, const std::string&);
        friend void connect(const std::string&);
        friend void output(Tuple);
        friend Tuple input(Pattern, std::chrono::milliseconds);
        friend Tuple read(Pattern, std::chrono::milliseconds);
    };
}

#endif //LINDA_STATE_H
