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

    class PatternDeserializationException : public PatternException{
    public:
        explicit PatternDeserializationException(const std::string& msg) : PatternException(msg){}
    };

    class PatternTypeException : public PatternException{
    public:
        explicit PatternTypeException(const std::string& msg) : PatternException(msg){}
    };

    class PatternSerializationCodeException : public PatternException{
    public:
        explicit PatternSerializationCodeException(const std::string& msg) : PatternException(msg){}
    };

    class PatternAnyException : public PatternException{
    public:
        explicit PatternAnyException(const std::string& msg) : PatternException(msg){}
    };

    class PatternFloatException : public PatternException{
    public:
        explicit PatternFloatException(const std::string& msg) : PatternException(msg){}
    };
}

#endif //LINDA_EXCEPTIONS_H
