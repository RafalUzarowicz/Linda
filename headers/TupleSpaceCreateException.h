#ifndef LINDA_TUPLESPACECREATEEXCEPTION_H
#define LINDA_TUPLESPACECREATEEXCEPTION_H

#include "TupleSpaceException.h"

namespace Linda{
    class TupleSpaceCreateException : public TupleSpaceException{
    public:
        explicit TupleSpaceCreateException(const std::string& msg)
                : TupleSpaceException("Could not create tuplespace." + msg) {}

    };
}


#endif //LINDA_TUPLESPACECREATEEXCEPTION_H
