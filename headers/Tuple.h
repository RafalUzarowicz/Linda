#ifndef LINDA_TUPLE_H
#define LINDA_TUPLE_H

#include <utility>
#include <vector>
#include <string>
#include <stdexcept>
#include <variant>
#include <sstream>
#include <cstring>

#include "ISerializable.h"
#include "Constants.h"
#include "Exceptions.h"

namespace Linda {
    template<class... Ts>
    struct operators : Ts ... {
        using Ts::operator()...;
    };
    template<class... Ts> operators(Ts...) -> operators<Ts...>;

    enum class TupleEntryType : size_t {
        Unknown = std::variant_npos,
        Int = 0,
        Float,
        String
    };

    class TupleEntry {
    public:
        using int_type = int32_t;
        using float_type = float;
        using string_type = std::string;
        using TupleValue = std::variant<int_type, float_type, string_type>;

        explicit TupleEntry(TupleValue value = 0) : value(std::move(value)) {}

        [[nodiscard]] const TupleValue& getValue() const {
            return value;
        }

        [[nodiscard]] TupleEntryType getType() const {
            switch (value.index()) {
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

        [[nodiscard]] std::string to_string() const;

    private:
        TupleValue value;
    };

    static const size_t INT_SIZE = sizeof(TupleEntry::int_type) / sizeof(ISerializable::serialization_type);
    static const size_t FLOAT_SIZE = sizeof(TupleEntry::float_type) / sizeof(ISerializable::serialization_type);

    class Tuple : public ISerializable {
    public:
        enum SerializationCodes {
            START = START_SERIALIZATION_CODE,
            END,
            INT,
            FLOAT,
            STRING
        };

        Tuple();

        ~Tuple() = default;

        Tuple(const Tuple&);

        explicit Tuple(const std::vector<ISerializable::serialization_type>&);

        using EntriesVector = std::vector<TupleEntry>;

        Linda::Tuple& push(TupleEntry::int_type);

        Linda::Tuple& push(TupleEntry::float_type);

        Linda::Tuple& push(TupleEntry::string_type);

        const EntriesVector& getEntries() const { return entries; }

        std::string path() const { return treePath.str(); }

        std::string to_string() const;

        void clear();

        TupleEntry& operator[](std::size_t index) { return entries[index]; }

        const TupleEntry& operator[](std::size_t index) const { return entries[index]; }

        size_t size() const { return entries.size(); }

        auto begin() { return entries.begin(); }

        auto end() { return entries.end(); }

        std::vector<serialization_type> serialize() override;

        void deserialize(const std::vector<serialization_type>& vector) override;

        void reset();

    private:
        EntriesVector entries;

        std::stringstream treePath;
    };
}

std::ostream& operator<<(std::ostream&, const Linda::TupleEntryType&);

std::ostream& operator<<(std::ostream&, const Linda::TupleEntry&);

std::ostream& operator<<(std::ostream&, const Linda::Tuple&);

#endif //LINDA_TUPLE_H