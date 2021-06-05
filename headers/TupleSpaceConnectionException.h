//
// Created by asia on 05.06.2021.
//

#ifndef LINDA_TUPLESPACECONNECTIONEXCEPTION_H
#define LINDA_TUPLESPACECONNECTIONEXCEPTION_H

class TupleSpaceConnectException : public std::runtime_error{
public:
    explicit TupleSpaceConnectException(const std::string& msg)
            : runtime_error("Could not connect to tuplespace." + msg) {}

};

#endif //LINDA_TUPLESPACECONNECTIONEXCEPTION_H
