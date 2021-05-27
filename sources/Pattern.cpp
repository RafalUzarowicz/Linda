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

std::vector<ISerializable::serialization_type> Linda::Pattern::serialize() {
    std::vector<serialization_type> data;
    data.emplace_back(Pattern::SerializationCodes::START);
    TupleEntry::int_type intTmp{};
    TupleEntry::float_type floatTmp{};
    TupleEntry::string_type strTmp{};
    Pattern::SerializationCodes entryType;
    for(auto& entry : entries){
        switch(entry.getTupleType()){
            case TupleEntryType::Int:
                entryType = INT;
                break;
            case TupleEntryType::Float:
                entryType = FLOAT;
                break;
            case TupleEntryType::String:
                entryType = STRING;
                break;
            default:
                break;
        }
    }
    data.emplace_back(Pattern::SerializationCodes::END);
    return data;
}

void Linda::Pattern::deserialize(const std::vector<serialization_type> &vector) {

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
