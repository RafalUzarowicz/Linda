#ifndef LINDA_EXCEPTIONS_H
#define LINDA_EXCEPTIONS_H

namespace Linda::Exception{
    class TupleSpaceException : public std::runtime_error{
    public:
        explicit TupleSpaceException(const std::string& msg)
                : runtime_error(msg) {}

    };

    class TupleSpaceCreateException : public TupleSpaceException{
    public:
        explicit TupleSpaceCreateException(const std::string& msg)
                : TupleSpaceException("Could not create tuplespace." + msg) {}

    };

    class TupleSpaceConnectException : public TupleSpaceException{
    public:
        explicit TupleSpaceConnectException(const std::string& msg)
                : TupleSpaceException("Could not connect to tuplespace." + msg) {}

    };

    class TupleException : public std::runtime_error{
    public:
        explicit TupleException(const std::string& msg) : std::runtime_error(msg){}
    };

    class TupleDeserializationError : public TupleException{
    public:
        explicit TupleDeserializationError(const std::string& msg) : TupleException(msg){}
    };

    class PatternException : public std::runtime_error{
    public:
        explicit PatternException(const std::string& msg) : std::runtime_error(msg){}
    };
}

#endif //LINDA_EXCEPTIONS_H
