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
                    [&](const std::string& str) {
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
    serializedLength = pattern.serializedLength;
    treePath << pattern.treePath.str();
}

Linda::Pattern::Pattern(const std::vector<ISerializable::serialization_type>& vector) {
    Pattern::deserialize(vector);
}

std::string Linda::Pattern::to_string() const {
    std::stringstream ss;
    ss << "(";
    for(size_t i = 0, e = entries.size(); i<e; ++i){
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

std::vector<std::string> Linda::Pattern::all_paths() const{
    std::vector<std::string> str_vec;
    str_vec.push_back(treePath.str());
    for(int i = entries.size()-1; i >=0; i--){
        if(entries[i].getType() != PatternEntryType::Any){
            break;
        }
        str_vec.push_back(treePath.str().substr(0,i));
    }
    return str_vec;
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
    add(PatternEntryType::Any, type);
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
            throw Linda::Exception::Pattern::TypeException("Unknown type for conversion.");
    }
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
            throw Linda::Exception::Pattern::SerializationCodeException("Unknown code for conversion.");
    }
}

void Linda::Pattern::add(Linda::PatternEntryType type, Linda::TupleEntry::int_type i) {
    if (type == PatternEntryType::Any) {
        throw Linda::Exception::Pattern::AnyException("Can't use Any with specific value.");
    }else{
        entries.emplace_back(type, i);
        serializedLength += 3 + INT_SIZE;
        treePath << "i";
    }
}

void Linda::Pattern::add(Linda::PatternEntryType type, Linda::TupleEntry::float_type f) {
    switch (type) {
        case PatternEntryType::Any:
            throw Linda::Exception::Pattern::AnyException("Can't use Any with specific value.");
        case PatternEntryType::Equal:
            throw Linda::Exception::Pattern::FloatException("Can't use Equal on float value.");
        default:
            entries.emplace_back(type, f);
            serializedLength += 3 + FLOAT_SIZE;
            treePath << "f";
            break;
    }
}

void Linda::Pattern::add(Linda::PatternEntryType type, Linda::TupleEntry::string_type s) {
    if (type == PatternEntryType::Any) {
        throw Linda::Exception::Pattern::AnyException("Can't use Any with specific value.");
    }else{
        entries.emplace_back(type, s);
        serializedLength += 3 + s.size();
        treePath << "s";
    }
}

void Linda::Pattern::add(PatternEntryType pType, Linda::TupleEntryType tType){
    if(pType != PatternEntryType::Any){
        throw Linda::Exception::Pattern::AnyException("TupleEntryType can be used only with PatternEntryType::Any.");
    }
    switch (tType) {
        case Linda::TupleEntryType::Int:
            entries.emplace_back(Linda::PatternEntryType::Any, 0);
            serializedLength += 3;
            treePath << 'i';
            break;
        case Linda::TupleEntryType::Float:
            entries.emplace_back(Linda::PatternEntryType::Any, 0.0f);
            serializedLength += 3;
            treePath << 'f';
            break;
        case Linda::TupleEntryType::String:
            entries.emplace_back(Linda::PatternEntryType::Any, "");
            serializedLength += 3;
            treePath << 's';
            break;
        default:
            throw Linda::Exception::Pattern::TypeException("Unknown tuple type.");
    }
}

std::vector<ISerializable::serialization_type> Linda::Pattern::serialize() {
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
    serializedLength = data.size();
    return data;
}

void Linda::Pattern::deserialize(const std::vector<serialization_type> &vector) {
    reset();

    serialization_type temp{};
    TupleEntry::int_type intTmp{};
    TupleEntry::float_type floatTmp{};
    TupleEntry::string_type strTmp{};

    TupleEntryType typeTmp{};
    SerializationCodes code{};
    PatternEntryType pTypeTmp{};

    if(!vector.empty() && vector.size() > 2){
        if(vector.front() != Pattern::SerializationCodes::START || vector.back() != Pattern::SerializationCodes::END){
            throw Linda::Exception::Pattern::DeserializationException("Wrong data format.");
        }else{
            for(size_t i{1}; i < vector.size()-1;){
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
                        throw Linda::Exception::Pattern::DeserializationException("Unknown serialization code.");
                }
                code = static_cast<SerializationCodes>(vector[i++]);
                temp = vector[i++];
                pTypeTmp = serializationCodeToType(static_cast<SerializationCodes>(temp));
                if(temp == ANY){
                    add<PatternEntryType::Any>(typeTmp);
                }else{
                    switch (typeTmp) {
                        case TupleEntryType::Int:
                            if(i + INT_SIZE >= vector.size()-1){
                                throw Linda::Exception::Pattern::DeserializationException("Not enough data for INT.");
                            }
                            memcpy(&intTmp, vector.data()+i, INT_SIZE);
                            add(pTypeTmp, intTmp);
                            i += INT_SIZE;
                            break;
                        case TupleEntryType::Float:
                            if(i + FLOAT_SIZE >= vector.size()-1){
                                throw Linda::Exception::Pattern::DeserializationException("Not enough data for FLOAT.");
                            }
                            memcpy(&floatTmp, vector.data()+i, FLOAT_SIZE);
                            add(pTypeTmp, floatTmp);
                            i += FLOAT_SIZE;
                            break;
                        case TupleEntryType::String:
                            strTmp.clear();
                            for(int32_t j{}; vector[i] != Pattern::SerializationCodes::STRING; ++j){
                                strTmp.push_back(vector[i++]);
                                if(j > Linda::MAX_STRING_LENGTH){
                                    throw Linda::Exception::Pattern::DeserializationException("STRING too long.");
                                }else if(i >= vector.size()-1){
                                    throw Linda::Exception::Pattern::DeserializationException("Wrong data format while reading STRING.");
                                }
                            }
                            add(pTypeTmp, strTmp);
                            break;
                    }
                }
                if(code != static_cast<SerializationCodes>(vector[i++])){
                    throw Linda::Exception::Pattern::DeserializationException("Wrong serialization code.");
                }
            }
        }
    }

    serializedLength = vector.size();
}

void Linda::Pattern::reset() {
    treePath.clear();
    serializedLength = 2;
    entries.clear();
}
