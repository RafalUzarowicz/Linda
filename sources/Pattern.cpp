#include "Pattern.h"

std::ostream& operator<<(std::ostream& stream, PatternEntryType type){
    switch (type) {
        case PatternEntryType::Equal:
            stream << "==";
            break;
        case PatternEntryType::Less:
            stream << "<";
            break;
        case PatternEntryType::LessOrEqual:
            stream << "<=";
            break;
        case PatternEntryType::Greater:
            stream << ">";
            break;
        case PatternEntryType::GreaterOrEqual:
            stream << ">=";
            break;
        case PatternEntryType::Any:
            stream << "*";
            break;
    }
    return stream;
}

std::ostream &operator<<(std::ostream& stream, const PatternEntry& patternEntry) {
    stream << patternEntry.to_string();
    return stream;
}

std::string PatternEntry::to_string() const {
    std::stringstream ss;
    ss << pattern.first;
    return std::visit(
            overloaded{
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

std::string Pattern::to_string() const {
    std::stringstream stringstream;
    stringstream << "(";
    for( size_t i = 0, e = patterns.size();; ++i ){
        stringstream << patterns[i].to_string();
        if(i<e-1){
            stringstream<<", ";
        }else{
            break;
        }
    }
    stringstream << ")";
    return stringstream.str();
}

std::ostream &operator<<(std::ostream& stream, const Pattern &pattern) {
    stream << pattern.to_string();
    return stream;
}

bool Pattern::check(Tuple) const {
    // TODO: implement
    return false;
}
