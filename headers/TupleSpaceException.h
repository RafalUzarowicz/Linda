//
// Created by asia on 05.06.2021.
//

#ifndef LINDA_TUPLESPACEEXCEPTION_H
#define LINDA_TUPLESPACEEXCEPTION_H

//todo make more specialized exceptions?
namespace Linda{
    class TupleSpaceException : public std::runtime_error{
    public:
        explicit TupleSpaceException(const std::string& msg)
                : runtime_error(msg) {}

    };
}

#endif //LINDA_TUPLESPACEEXCEPTION_H
