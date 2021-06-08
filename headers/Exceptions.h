#ifndef LINDA_EXCEPTIONS_H
#define LINDA_EXCEPTIONS_H

namespace Linda::Exception{
    class TupleSpaceException : public std::runtime_error{
    public:
        explicit TupleSpaceException(const std::string& msg)
                : std::runtime_error(msg) {}

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

    namespace Tuple{
        class BaseException : public std::runtime_error{
        public:
            explicit BaseException(const std::string& msg) : std::runtime_error(msg){}
        };

        class DeserializationError : public BaseException{
        public:
            explicit DeserializationError(const std::string& msg) : BaseException(msg){}
        };
    }

    namespace Pattern{
        class BaseException : public std::runtime_error{
        public:
            explicit BaseException(const std::string& msg) : std::runtime_error(msg){}
        };

        class DeserializationException : public BaseException{
        public:
            explicit DeserializationException(const std::string& msg) : BaseException(msg){}
        };

        class TypeException : public BaseException{
        public:
            explicit TypeException(const std::string& msg) : BaseException(msg){}
        };

        class SerializationCodeException : public BaseException{
        public:
            explicit SerializationCodeException(const std::string& msg) : BaseException(msg){}
        };

        class AnyException : public BaseException{
        public:
            explicit AnyException(const std::string& msg) : BaseException(msg){}
        };

        class FloatException : public BaseException{
        public:
            explicit FloatException(const std::string& msg) : BaseException(msg){}
        };
    }

    namespace Signal{
        class BaseException : public std::runtime_error{
        public:
            explicit BaseException(const std::string& msg) : std::runtime_error(msg){}
        };

        class EncodingError : public BaseException{
        public:
            explicit EncodingError(const std::string& msg) : BaseException(msg){}
        };
    }
}

#endif //LINDA_EXCEPTIONS_H
