//
// Created by Joanna Sokołowska on 8.06.2021.
//
#include "Linda.h"
#ifndef LINDA_CLI_H
#define LINDA_CLI_H

namespace Linda{
    class CLI {
    public:
        void run();
    private:
        Tuple t;
        Pattern p;
        std::vector<std::string> tokens;
        std::string line;
        std::string line_cpy;

        bool handleCreate();
        bool handleConnect();
        bool handleTuple();
        bool handlePattern();
        bool handleOperations();
        static void printHelp();
        void reset();
        void tokenize();
        void show();
        PatternEntryType parsePatternEntry();
    };
}



#endif //LINDA_CLI_H
