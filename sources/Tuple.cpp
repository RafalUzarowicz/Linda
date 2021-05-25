#include "Tuple.h"

std::ostream &operator<<(std::ostream& stream, const Linda::TupleEntryType& type) {
    switch (type) {
        case Linda::TupleEntryType::Int:
            return stream << "int";
        case Linda::TupleEntryType::Float:
            return stream << "float";
        case Linda::TupleEntryType::String:
            return stream << "string";
        case Linda::TupleEntryType::Unknown:
            return stream << "unknown";
    }
    return stream;
}

std::ostream& operator<<(std::ostream& stream, const Linda::TupleEntry& tupleEntry){
    stream << tupleEntry.to_string();
    return stream;
}

std::string Linda::TupleEntry::to_string() const {
    return std::visit(
            operators{
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

Linda::Tuple::Tuple(const Tuple& tuple) {
    entries = tuple.entries;
    treePath << tuple.treePath.str();
}

void Linda::Tuple::push(int value){
    entries.emplace_back(value);
    treePath << 'i';
}

void Linda::Tuple::push(float value){
    entries.emplace_back(value);
    treePath << 'f';
}

void Linda::Tuple::push(std::string value){
    entries.emplace_back(value);
    treePath << 's';
}

std::string Linda::Tuple::to_string() const{
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

std::ostream& operator<<(std::ostream& stream, const Linda::Tuple& tuple){
    stream << tuple.to_string();
    return stream;
}