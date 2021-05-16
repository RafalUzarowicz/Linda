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
    }
    return stream;
}

bool Pattern::check(Tuple) const {
    // TODO: implement
    return false;
}
