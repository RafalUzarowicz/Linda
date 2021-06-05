#ifndef LINDA_TUPLESPACECONNECTIONEXCEPTION_H
#define LINDA_TUPLESPACECONNECTIONEXCEPTION_H

#include "TupleSpaceException.h"

namespace Linda{
    class TupleSpaceConnectException : public TupleSpaceException{
    public:
        explicit TupleSpaceConnectException(const std::string& msg)
                : TupleSpaceException("Could not connect to tuplespace." + msg) {}

    };
}


#endif //LINDA_TUPLESPACECONNECTIONEXCEPTION_H
