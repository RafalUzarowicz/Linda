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
                    [](const TupleEntry::string_type& str) {
                        return "s:\""+str+"\"";
                    }
            },
            value
    );
}

Linda::Tuple::Tuple(const Tuple& tuple) {
    entries = tuple.entries;
    serializedLength = tuple.serializedLength;
    treePath << tuple.treePath.str();
}

Linda::Tuple::Tuple(const std::vector<ISerializable::serialization_type>& vec) {
    Tuple::deserialize(vec);
}

Linda::Tuple& Linda::Tuple::push(TupleEntry::int_type value){
    entries.emplace_back(value);
    treePath << 'i';
    serializedLength += 2 + INT_SIZE;
    return *this;
}

Linda::Tuple& Linda::Tuple::push(TupleEntry::float_type value){
    entries.emplace_back(value);
    treePath << 'f';
    serializedLength += 2 + FLOAT_SIZE;
    return *this;
}

Linda::Tuple& Linda::Tuple::push(TupleEntry::string_type value){
    entries.emplace_back(value);
    treePath << 's';
    serializedLength += 2 + value.size();
    return *this;
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
    data.emplace_back(Tuple::SerializationCodes::START);
    TupleEntry::int_type intTmp{};
    TupleEntry::float_type floatTmp{};
    TupleEntry::string_type strTmp{};
    for(auto& entry : entries){
           switch(entry.getType()){
               case TupleEntryType::Int:
                   data.emplace_back(Tuple::SerializationCodes::INT);
                   intTmp = std::get<TupleEntry::int_type>(entry.getValue());
                   data.insert(data.end(), (serialization_type*) & intTmp, (serialization_type *) & intTmp + sizeof(TupleEntry::int_type));
                   data.emplace_back(Tuple::SerializationCodes::INT);
                   break;
               case TupleEntryType::Float:
                   data.emplace_back(Tuple::SerializationCodes::FLOAT);
                   floatTmp = std::get<TupleEntry::float_type>(entry.getValue());
                   data.insert(data.end(), (serialization_type*) & floatTmp, (serialization_type *) & floatTmp + sizeof(TupleEntry::float_type));
                   data.emplace_back(Tuple::SerializationCodes::FLOAT);
                   break;
               case TupleEntryType::String:
                   data.emplace_back(Tuple::SerializationCodes::STRING);
                   strTmp = std::get<TupleEntry::string_type>(entry.getValue());
                   data.insert(data.end(), strTmp.data(), strTmp.data() + strTmp.size());
                   data.emplace_back(Tuple::SerializationCodes::STRING);
                   break;
               default:
                   break;
           }
    }
    data.emplace_back(Tuple::SerializationCodes::END);
    serializedLength = data.size();
    return data;
}

void Linda::Tuple::deserialize(const std::vector<serialization_type>& vector) {
    reset();

    serialization_type temp{};
    TupleEntry::int_type intTmp{};
    TupleEntry::float_type floatTmp{};
    TupleEntry::string_type strTmp{};

    if(!vector.empty() && vector.size() > 2){
        if(vector.front() != Tuple::SerializationCodes::START || vector.back() != Tuple::SerializationCodes::END){
            throw Linda::Exception::Tuple::DeserializationError("Wrong data format.");
        }else{
            for(size_t i{1}; i < vector.size()-1;){
                temp = vector[i++];
                switch (temp) {
                    case Tuple::SerializationCodes::INT:
                        if(i + INT_SIZE >= vector.size()-1){
                            throw Linda::Exception::Tuple::DeserializationError("Not enough data for INT.");
                        }
                        memcpy(&intTmp, vector.data()+i, INT_SIZE);
                        push(intTmp);
                        i += INT_SIZE;
                        if(vector[i] != Tuple::SerializationCodes::INT){
                            throw Linda::Exception::Tuple::DeserializationError("Wrong data format while reading INT.");
                        }else{
                            ++i;
                        }
                        break;
                    case Tuple::SerializationCodes::FLOAT:
                        if(i + FLOAT_SIZE >= vector.size()-1){
                            throw Linda::Exception::Tuple::DeserializationError("Not enough data for FLOAT.");
                        }
                        memcpy(&floatTmp, vector.data()+i, FLOAT_SIZE);
                        push(floatTmp);
                        i += FLOAT_SIZE;
                        if(vector[i] != Tuple::SerializationCodes::FLOAT){
                            throw Linda::Exception::Tuple::DeserializationError("Wrong data format while reading FLOAT.");
                        }else{
                            ++i;
                        }
                        break;
                    case Tuple::SerializationCodes::STRING:
                        strTmp.clear();
                        for(int32_t j{}; vector[i] != Tuple::SerializationCodes::STRING; ++j){
                            strTmp.push_back(vector[i++]);
                            if( j > Linda::MAX_STRING_LENGTH){
                                throw Linda::Exception::Tuple::DeserializationError("STRING too long.");
                            }else if(i >= vector.size()-1){
                                throw Linda::Exception::Tuple::DeserializationError("Wrong data format while reading STRING.");
                            }
                        }
                        push(strTmp);
                        ++i;
                        break;
                    default:
                        throw Linda::Exception::Tuple::DeserializationError("Unknown serialization code.");
                }
            }
        }
    }

    serializedLength = vector.size();
}

void Linda::Tuple::reset() {
    treePath.clear();
    serializedLength = 2;
    entries.clear();
}

Linda::Tuple::Tuple() {
    reset();
}

std::ostream& operator<<(std::ostream& stream, const Linda::Tuple& tuple){
    stream << tuple.to_string();
    return stream;
}