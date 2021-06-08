#ifndef LINDA_STATE_H
#define LINDA_STATE_H

#include "Constants.h"

namespace Linda{
    class State{
    public:
        State(State&) = delete;
        void operator=(const State&) = delete;
    private:
        State() : tupleSpacePath(DEFAULT_TUPLESPACE_DIR + "/" + DEFAULT_TUPLESPACE_NAME) {
            connected = false;
            depth = 0;
            index = 0;
        }
        static State & getInstance(){
            static State instance;
            return instance;
        }

        //tuplespace  = a dir named tuplespace in specified path, the dir contains file .tuplespace
        std::string tupleSpacePath{};

        bool connected;
        int depth;
        int index;

        friend void create(bool, const std::string&, const std::string&);
        friend void connect(const std::string&);
        friend void output(Tuple);
        friend Tuple input(Pattern, std::chrono::milliseconds);
        friend Tuple read(Pattern pattern, std::chrono::milliseconds timeout);
        friend void sighandler(int signum, siginfo_t *info, void *ptr);
        friend Tuple wait_for_it(const Pattern& pattern, char type);
    };
}

#endif //LINDA_STATE_H
