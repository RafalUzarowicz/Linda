#ifndef LINDA_ISERIALIZABLE_H
#define LINDA_ISERIALIZABLE_H

#include <vector>

class ISerializable{
public:
    using serialization_type = unsigned char;
    virtual std::vector<serialization_type> serialize() = 0;
    virtual void deserialize(const std::vector<serialization_type>&) = 0;
    virtual size_t getSerializedLength(){ return serializedLength; };

protected:
    size_t serializedLength{};
};

#endif //LINDA_ISERIALIZABLE_H
