#include "Pattern.h"

std::ostream &operator<<(std::ostream& stream, const Linda::PatternEntry& patternEntry) {
    stream << patternEntry.to_string();
    return stream;
}

std::ostream &operator<<(std::ostream& stream, const Linda::Pattern &pattern) {
    stream << pattern.to_string();
    return stream;
}

std::ostream& operator<<(std::ostream& stream, const Linda::PatternEntryType& type){
    switch (type) {
        case Linda::PatternEntryType::Equal:
            return stream << "==";
        case Linda::PatternEntryType::Less:
            return stream << "<";
        case Linda::PatternEntryType::LessOrEqual:
            return stream << "<=";
        case Linda::PatternEntryType::Greater:
            return stream << ">";
        case Linda::PatternEntryType::GreaterOrEqual:
            return stream << ">=";
        case Linda::PatternEntryType::Any:
            return stream << "*";
    }
    return stream;
}

[[nodiscard]] const Linda::PatternEntry::PatternValue& Linda::PatternEntry::getPattern() const {
    return pattern;
}

[[nodiscard]] Linda::TupleEntryType Linda::PatternEntry::getTupleType() const {
    switch (pattern.second.index()) {
        case 0:
            return TupleEntryType::Int;
        case 1:
            return TupleEntryType::Float;
        case 2:
            return TupleEntryType::String;
        default:
            return TupleEntryType::Unknown;
    }
}

std::string Linda::PatternEntry::to_string() const {
    std::stringstream ss;
    ss << pattern.first;
    return std::visit(
            operators{
                    [&](int i) {
                        if(ss.str() == "*"){
                            return std::string("i:*");
                        }else{
                            return std::string("i:")+ss.str()+std::to_string(i);
                        }
                    },
                    [&](float f) {
                        if(ss.str() == "*"){
                            return std::string("f:*");
                        }else{
                            std::string str = std::to_string(f);
                            return std::string("f:")+ss.str()+str.erase(str.find_last_not_of('0') + 2, std::string::npos);
                        }
                    },
                    [&](std::string str) {
                        if(ss.str() == "*"){
                            return std::string("s:*");
                        }else{
                            return "s:"+ss.str()+"\""+str+"\"";
                        }
                    }
            },
            pattern.second
    );
}

Linda::PatternEntryType Linda::PatternEntry::getType() const {
    return pattern.first;
}

Linda::PatternEntry::PatternEntry(const Linda::PatternEntry& patternEntry) = default;

Linda::Pattern::Pattern(const Linda::Pattern& pattern) : entries(pattern.entries){
    treePath << pattern.treePath.str();
}

Linda::Pattern::Pattern(const std::vector<ISerializable::serialization_type>& vector) {
    Pattern::deserialize(vector);
}

std::string Linda::Pattern::to_string() const {
    std::stringstream ss;
    ss << "(";
    for(size_t i = 0, e = entries.size();; ++i ){
        ss << entries[i].to_string();
        if(i<e-1){
            ss << ", ";
        }else{
            break;
        }
    }
    ss << ")";
    return ss.str();
}

bool Linda::Pattern::check(const Linda::Tuple& tuple) const {
    if(tuple.size() > size() || treePath.str().find(tuple.path()) != 0) return false;
    for( size_t i{}; i<tuple.size(); ++i){
        if(tuple[i].getType() == entries[i].getTupleType()){
            switch (entries[i].getType()) {
                case PatternEntryType::Equal:
                    if(tuple[i].getValue() != entries[i].getValue()) return false;
                    break;
                case PatternEntryType::Less:
                    if(tuple[i].getValue() >= entries[i].getValue()) return false;
                    break;
                case PatternEntryType::LessOrEqual:
                    if(tuple[i].getValue() > entries[i].getValue()) return false;
                    break;
                case PatternEntryType::Greater:
                    if(tuple[i].getValue() <= entries[i].getValue()) return false;
                    break;
                case PatternEntryType::GreaterOrEqual:
                    if(tuple[i].getValue() < entries[i].getValue()) return false;
                    break;
                default:
                    break;
            }
        }else{
            return false;
        }
    }
    return true;
}

template<>
void Linda::Pattern::add<Linda::PatternEntryType::Any>(Linda::TupleEntryType type) {
    switch (type) {
        case Linda::TupleEntryType::Int:
            entries.emplace_back(Linda::PatternEntryType::Any, 0);
            treePath << 'i';
            break;
        case Linda::TupleEntryType::Float:
            entries.emplace_back(Linda::PatternEntryType::Any, 0.0f);
            treePath << 'f';
            break;
        case Linda::TupleEntryType::String:
            entries.emplace_back(Linda::PatternEntryType::Any, "");
            treePath << 's';
            break;
        default:
            // TODO: custom exceptions
            throw std::runtime_error("Unknown type!");
    }
}

Linda::Pattern::SerializationCodes Linda::Pattern::typeToSerializationCode(Linda::PatternEntryType type) {
    switch (type) {
        case PatternEntryType::Equal:
            return EQUAL;
        case PatternEntryType::Less:
            return LESS;
        case PatternEntryType::LessOrEqual:
            return LESS_OR_EQUAL;
        case PatternEntryType::Greater:
            return GREATER;
        case PatternEntryType::GreaterOrEqual:
            return GREATER_OR_EQUAL;
        case PatternEntryType::Any:
            return ANY;
        default:
            break;
    }
    // TODO: exception at wrong type
    return ANY;
}

Linda::PatternEntryType Linda::Pattern::serializationCodeToType(SerializationCodes code) {
    switch (code) {
        case EQUAL:
            return PatternEntryType::Equal;
        case LESS:
            return PatternEntryType::Less;
        case LESS_OR_EQUAL:
            return PatternEntryType::LessOrEqual;
        case GREATER:
            return PatternEntryType::Greater;
        case GREATER_OR_EQUAL:
            return PatternEntryType::GreaterOrEqual;
        case ANY:
            return PatternEntryType::Any;
        default:
            break;
    }
    // TODO: exception at wrong code
    return PatternEntryType::Any;
}

void Linda::Pattern::addWithoutChecks(Linda::PatternEntryType type, Linda::TupleEntry::int_type i) {
    entries.emplace_back(type, i);
}

void Linda::Pattern::addWithoutChecks(Linda::PatternEntryType type, Linda::TupleEntry::float_type f) {
    entries.emplace_back(type, f);
}

void Linda::Pattern::addWithoutChecks(Linda::PatternEntryType type, Linda::TupleEntry::string_type s) {
    entries.emplace_back(type, s);
}

std::vector<ISerializable::serialization_type> Linda::Pattern::serialize() {
    // TODO: refactor
    std::vector<serialization_type> data;
    data.emplace_back(Pattern::SerializationCodes::START);
    TupleEntry::int_type intTmp{};
    TupleEntry::float_type floatTmp{};
    TupleEntry::string_type strTmp{};
    SerializationCodes entryType;
    for(auto& entry : entries){
        entryType = typeToSerializationCode(entry.getType());
        switch(entry.getTupleType()){
            case TupleEntryType::Int:
                data.emplace_back(Pattern::SerializationCodes::INT);
                data.emplace_back(entryType);
                if(entryType != ANY) {
                    intTmp = std::get<TupleEntry::int_type>(entry.getValue());
                    data.insert(data.end(), (serialization_type * ) & intTmp,
                                (serialization_type * ) & intTmp + sizeof(TupleEntry::int_type));
                }
                data.emplace_back(Pattern::SerializationCodes::INT);
                break;
            case TupleEntryType::Float:
                data.emplace_back(Pattern::SerializationCodes::FLOAT);
                data.emplace_back(entryType);
                if(entryType != ANY) {
                    floatTmp = std::get<TupleEntry::float_type>(entry.getValue());
                    data.insert(data.end(), (serialization_type * ) & floatTmp,
                                (serialization_type * ) & floatTmp + sizeof(TupleEntry::float_type));
                }
                data.emplace_back(Pattern::SerializationCodes::FLOAT);
                break;
            case TupleEntryType::String:
                data.emplace_back(Pattern::SerializationCodes::STRING);
                data.emplace_back(entryType);
                if(entryType != ANY) {
                    strTmp = std::get<TupleEntry::string_type>(entry.getValue());
                    data.insert(data.end(), strTmp.data(), strTmp.data() + strTmp.size());
                }
                data.emplace_back(Pattern::SerializationCodes::STRING);
                break;
            default:
                break;
        }
    }
    data.emplace_back(Pattern::SerializationCodes::END);
    return data;
}

void Linda::Pattern::deserialize(const std::vector<serialization_type> &vector) {
    // TODO: custom exceptions instead of bool
    // TODO: refactor
    serialization_type temp{};
    TupleEntry::int_type intTmp{};
    TupleEntry::float_type floatTmp{};
    TupleEntry::string_type strTmp{};

    TupleEntryType typeTmp{};
    SerializationCodes code{};
    PatternEntryType pTypeTmp{};

    bool isDeserializable = true;
    if(!vector.empty() && vector.size() > 2){
        if(vector.front() != Pattern::SerializationCodes::START || vector.back() != Pattern::SerializationCodes::END){
            isDeserializable = false;
        }else{
            for(size_t i{1}; isDeserializable && i < vector.size()-1;){
                switch (vector[i]) {
                    case Pattern::SerializationCodes::INT:
                        typeTmp = TupleEntryType::Int;
                        break;
                    case Pattern::SerializationCodes::FLOAT:
                        typeTmp = TupleEntryType::Float;
                        break;
                    case Pattern::SerializationCodes::STRING:
                        typeTmp = TupleEntryType::String;
                        break;
                    default:
                        isDeserializable = false;
                        break;
                }
                if(isDeserializable){
                    code = static_cast<SerializationCodes>(vector[i++]);
                    temp = vector[i++];
                    pTypeTmp = static_cast<PatternEntryType>(temp);
                    if(temp == ANY){
                        add<PatternEntryType::Any>(typeTmp);
                    }else{
                        switch (typeTmp) {
                            case TupleEntryType::Int:
                                memcpy(&intTmp, vector.data()+i, sizeof(TupleEntry::int_type) / sizeof(serialization_type));
                                addWithoutChecks(pTypeTmp, intTmp);
                                i += sizeof(TupleEntry::int_type) / sizeof(serialization_type);
                                if(i >= vector.size()-1){
                                    isDeserializable = false;
                                }
                                break;
                            case TupleEntryType::Float:
                                memcpy(&floatTmp, vector.data()+i, sizeof(TupleEntry::float_type) / sizeof(serialization_type));
                                addWithoutChecks(pTypeTmp, floatTmp);
                                i += sizeof(TupleEntry::float_type) / sizeof(serialization_type);
                                if(i >= vector.size()-1){
                                    isDeserializable = false;
                                }
                                break;
                            case TupleEntryType::String:
                                strTmp.clear();
                                // TODO: limit maximum string
                                while(vector[i] != Pattern::SerializationCodes::STRING){
                                    strTmp.push_back(vector[i]);
                                    ++i;
                                    if(i >= vector.size()-1){
                                        isDeserializable = false;
                                        break;
                                    }
                                }
                                if(isDeserializable){
                                    addWithoutChecks(pTypeTmp, strTmp);
                                }
                                break;
                            case TupleEntryType::Unknown:
                                isDeserializable = false;
                                break;
                        }
                    }
                }
                if(code != static_cast<SerializationCodes>(vector[i++])){
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
