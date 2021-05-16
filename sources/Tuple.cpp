#include "Tuple.h"

std::ostream& operator<<(std::ostream& stream, const TupleEntry& tupleEntry){
    stream << std::visit(
            overloaded{
                    [](int i) {
                        return std::string("i:")+std::to_string(i);
                    },
                    [](float f) {
                        std::string str = std::to_string(f);
                        return std::string("f:")+str.erase(str.find_last_not_of('0') + 2, std::string::npos);
                    },
                    [](std::string str) {
                        return "s:\""+str+"\"";
                    }
            },
            tupleEntry.getValue()
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

std::string Tuple::to_string() const{
    std::stringstream stringstream;
    stringstream << "(";
    for( size_t i = 0, e = entries.size();; ++i ){
        stringstream << entries[i];
        if(i<e-1){
            stringstream<<", ";
        }else{
            break;
        }
    }
    stringstream << ")";
    return stringstream.str();
}

std::ostream& operator<<(std::ostream& stream, const Tuple& tuple){
    stream << tuple.to_string();
    return stream;
}