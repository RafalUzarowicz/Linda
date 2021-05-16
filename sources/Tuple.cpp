#include "Tuple.h"

std::ostream &operator<<(std::ostream& stream, TupleEntryType type) {
    switch (type) {
        case TupleEntryType::Int:
            stream << "int";
            break;
        case TupleEntryType::Float:
            stream << "float";
            break;
        case TupleEntryType::String:
            stream << "string";
            break;
        case TupleEntryType::Unknown:
            stream << "unknown";
            break;
    }
    return stream;
}

std::ostream& operator<<(std::ostream& stream, const TupleEntry& tupleEntry){
    stream << tupleEntry.to_string();
    return stream;
}

std::string TupleEntry::to_string() const {
    return std::visit(
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
            value
    );
}

Tuple::Tuple(const Tuple& tuple) {
    entries = tuple.entries;
    treePath << tuple.treePath.str();
}

void Tuple::push(int value){
    entries.emplace_back(value);
    treePath << 'i';
}

void Tuple::push(float value){
    entries.emplace_back(value);
    treePath << 'f';
}

void Tuple::push(std::string value){
    entries.emplace_back(value);
    treePath << 's';
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