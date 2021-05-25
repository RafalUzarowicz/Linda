#pragma once

#include <utility>
#include <vector>
#include <string>
#include <stdexcept>
#include <variant>
#include <sstream>

namespace Linda{
    template<class... Ts> struct operators : Ts... { using Ts::operator()...; };
    template<class... Ts> operators(Ts...) -> operators<Ts...>;

    enum class TupleEntryType : size_t{
        Unknown = std::variant_npos,
        Int = 0,
        Float,
        String
    };

    class TupleEntry{
    public:
        using TupleValue = std::variant<int, float, std::string>;

        explicit TupleEntry(TupleValue value = 0) : value(std::move(value)){}

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

        std::string to_string() const;

    private:
        TupleValue value;
    };

    class Tuple {
    public:
        Tuple() = default;
        ~Tuple() = default;
        Tuple(const Tuple&);

        using EntriesVector = std::vector<TupleEntry>;

        void push(int);

        void push(float);

        void push(std::string);

        const EntriesVector& getEntries() const { return entries; }
        std::string path() const { return treePath.str(); }
        std::string to_string() const;

        void clear(){ entries.clear(); treePath.clear(); }

        TupleEntry& operator[](std::size_t index){ return entries[index]; }
        const TupleEntry& operator[](std::size_t index) const { return entries[index]; }
        size_t size() const { return entries.size(); }
        auto begin() { return entries.begin(); }
        auto end() { return entries.end(); }

    private:
        EntriesVector entries;
        std::stringstream treePath;
    };
}

std::ostream& operator<<(std::ostream&, const Linda::TupleEntryType&);
std::ostream& operator<<(std::ostream&, const Linda::TupleEntry&);
std::ostream& operator<<(std::ostream&, const Linda::Tuple&);
