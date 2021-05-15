#include "Tuple.h"

std::ostream& operator<<(std::ostream& stream, const TupleEntry& tupleEntry){
    stream << std::visit(
            overloaded{
                    [](int b) {
                        return std::string("int:")+std::to_string(b);
                    },
                    [](float b) {
                        return std::string("float:")+std::to_string(b);
                    },
                    [](std::string b) {
                        return std::string("string:")+b;
                    }
            },
            tupleEntry.value
    );
    return stream;
}

template<>
void Tuple::push<int>(int value){
    entries.emplace_back(value);
}

template<>
void Tuple::push<float>(float value){
    entries.emplace_back(value);
}

template<>
void Tuple::push<std::string>(std::string value){
    entries.emplace_back(value);
}