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

Linda::PatternEntry::PatternEntry(const Linda::PatternEntry& patternEntry) {
    pattern = patternEntry.pattern;
}

Linda::Pattern::Pattern(const Linda::Pattern& pattern) {
    entries = pattern.entries;
    treePath.clear();
    treePath << pattern.treePath.str();
}

std::string Linda::Pattern::to_string() const {
    std::stringstream stringstream;
    stringstream << "(";
    for(size_t i = 0, e = entries.size();; ++i ){
        stringstream << entries[i].to_string();
        if(i<e-1){
            stringstream<<", ";
        }else{
            break;
        }
    }
    stringstream << ")";
    return stringstream.str();
}

bool Linda::Pattern::check(const Linda::Tuple& tuple) const {
    if(tuple.size() > size()) return false;
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
            break;
        case Linda::TupleEntryType::Float:
            entries.emplace_back(Linda::PatternEntryType::Any, 0.0f);
            break;
        case Linda::TupleEntryType::String:
            entries.emplace_back(Linda::PatternEntryType::Any, "");
            break;
        default:
            throw std::runtime_error("Unknown type!");
    }
}
