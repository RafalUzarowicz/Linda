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
                    [](TupleEntry::int_type i) {
                        return std::string("i:")+std::to_string(i);
                    },
                    [](TupleEntry::float_type f) {
                        std::string str = std::to_string(f);
                        return std::string("f:")+str.erase(str.find_last_not_of('0') + 2, std::string::npos);
                    },
                    [](TupleEntry::string_type str) {
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

Linda::Tuple::Tuple(const std::vector<char>& vec) {
    Tuple::deserialize(vec);
}

void Linda::Tuple::push(TupleEntry::int_type value){
    entries.emplace_back(value);
    treePath << 'i';
}

void Linda::Tuple::push(TupleEntry::float_type value){
    entries.emplace_back(value);
    treePath << 'f';
}

void Linda::Tuple::push(TupleEntry::string_type value){
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

std::vector<ISerializable::serialization_type> Linda::Tuple::serialize() {
    std::vector<serialization_type> data;
    data.emplace_back(START);
    TupleEntry::int_type intTmp{};
    TupleEntry::float_type floatTmp{};
    TupleEntry::string_type strTmp{};
    for(auto& entry : entries){
           switch(entry.getType()){
               case TupleEntryType::Int:
                   data.emplace_back(INT);
                   intTmp = std::get<TupleEntry::int_type>(entry.getValue());
                   data.insert(data.end(), (serialization_type*) & intTmp, (serialization_type *) & intTmp + sizeof(TupleEntry::int_type));
                   data.emplace_back(INT);
                   break;
               case TupleEntryType::Float:
                   data.emplace_back(FLOAT);
                   floatTmp = std::get<TupleEntry::float_type>(entry.getValue());
                   data.insert(data.end(), (serialization_type*) & floatTmp, (serialization_type *) & floatTmp + sizeof(TupleEntry::float_type));
                   data.emplace_back(FLOAT);
                   break;
               case TupleEntryType::String:
                   data.emplace_back(STRING);
                   strTmp = std::get<TupleEntry::string_type>(entry.getValue());
                   data.insert(data.end(), strTmp.data(), strTmp.data() + strTmp.size());
                   data.emplace_back(STRING);
                   break;
               default:
                   break;
           }
    }
    data.emplace_back(END);
    return data;
}

void Linda::Tuple::deserialize(const std::vector<serialization_type>& vector) {
    serialization_type temp{};
    TupleEntry::int_type intTmp{};
    TupleEntry::float_type floatTmp{};
    TupleEntry::string_type strTmp{};
    bool isDeserializable = true;
    if(!vector.empty() && vector.size() > 2){
        if(vector.front() != START || vector.back() != END){
            isDeserializable = false;
        }else{
            for(size_t i{1}; isDeserializable && i < vector.size()-1;){
                temp = vector[i];
                ++i;
                switch (temp) {
                    case INT:
                        memcpy(&intTmp, vector.data()+i, sizeof(TupleEntry::int_type) / sizeof(serialization_type));
                        push(intTmp);
                        i += sizeof(TupleEntry::int_type) / sizeof(serialization_type);
                        if(i >= vector.size()-1 || vector[i] != INT){
                            isDeserializable = false;
                            break;
                        }else{
                            ++i;
                        }
                        break;
                    case FLOAT:
                        memcpy(&floatTmp, vector.data()+i, sizeof(TupleEntry::float_type) / sizeof(serialization_type));
                        push(floatTmp);
                        i += sizeof(TupleEntry::float_type) / sizeof(serialization_type);
                        if(i >= vector.size()-1 || vector[i] != FLOAT){
                            isDeserializable = false;
                            break;
                        }else{
                            ++i;
                        }
                        break;
                    case STRING:
                        strTmp.clear();
                        // TODO: limit maximum string
                        while(vector[i] != STRING){
                            strTmp.push_back(vector[i]);
                            ++i;
                            if(i >= vector.size()-1){
                                isDeserializable = false;
                                break;
                            }
                        }
                        if(isDeserializable){
                            push(strTmp);
                            ++i;
                        }
                        break;
                    default:
                        isDeserializable = false;
                        break;
                }
            }
        }
    }
    if(!isDeserializable){
        entries.clear();
    }
}

std::ostream& operator<<(std::ostream& stream, const Linda::Tuple& tuple){
    stream << tuple.to_string();
    return stream;
}