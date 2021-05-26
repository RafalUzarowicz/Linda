#ifndef LINDA_ISERIALIZABLE_H
#define LINDA_ISERIALIZABLE_H

#include <vector>

class ISerializable{
public:
    using serialization_type = char;
    virtual std::vector<serialization_type> serialize() = 0;
    virtual void deserialize(const std::vector<serialization_type>&) = 0;
};

#endif //LINDA_ISERIALIZABLE_H
