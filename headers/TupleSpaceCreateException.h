//
// Created by asia on 05.06.2021.
//

#ifndef LINDA_TUPLESPACECREATEEXCEPTION_H
#define LINDA_TUPLESPACECREATEEXCEPTION_H

//todo add more exceptions
class TupleSpaceCreateException : public std::runtime_error{
public:
    explicit TupleSpaceCreateException(const std::string& msg)
            : runtime_error("Could not create tuplespace." + msg) {}

};

#endif //LINDA_TUPLESPACECREATEEXCEPTION_H
