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

        bool handle_create();
        bool handle_connect();
        bool handle_tuple();
        bool handle_pattern();
        bool handle_operations();
        void print_help();
        void reset();
        void tokenize();
    };
}



#endif //LINDA_CLI_H
