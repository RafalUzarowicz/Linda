#include <gtest/gtest.h>
#include <iostream>
#include <variant>
#include "Tuple.h"
#include "Pattern.h"
#include "Exceptions.h"

TEST(TupleTest, Types) {
    Linda::Tuple tuple(Linda::Tuple().push(2).push(3.0f).push("Tak"));
    const Linda::TupleEntryType expectedTypes[] = {
            Linda::TupleEntryType::Int,
            Linda::TupleEntryType::Float,
            Linda::TupleEntryType::String
    };

    int i = 0;
    for (auto &val : tuple) {
        EXPECT_EQ(val.getType(), expectedTypes[i]);
        i++;
    }
}

TEST(TupleTest, Path) {
    Linda::Tuple tuple(Linda::Tuple().push(2).push(3.0f).push("Tak"));
    const std::string expectedPath = "ifs";

    EXPECT_EQ(tuple.path(), expectedPath);
}

TEST(TupleTest, SerializationDeserialization) {
    Linda::Tuple tuple(Linda::Tuple().push(2).push(3.0f).push("Tak"));
    const unsigned char ExpectedSerialization[] = {128, 130, 2, 0, 0, 0, 130, 131, 0, 0, 64, 64, 131, 132, 84, 97, 107,
                                                   132, 129};

    int i = 0;
    for (auto &c : tuple.serialize()) {
        EXPECT_EQ(static_cast<unsigned>(c), ExpectedSerialization[i]);
        i++;
    }

    Linda::Tuple tuple2(tuple.serialize());

    EXPECT_EQ(tuple.to_string(), tuple2.to_string());
}
//TODO: Tuple exceptions

// PATTERN TESTS
TEST(PatternTest, Any) {
    Linda::Pattern pattern;
    pattern.add<Linda::PatternEntryType::Any>(Linda::TupleEntryType::String);
    pattern.add<Linda::PatternEntryType::Any>(Linda::TupleEntryType::Int);
    pattern.add<Linda::PatternEntryType::Any>(Linda::TupleEntryType::Float);
    const Linda::TupleEntryType expectedTypes[] = {
            Linda::TupleEntryType::String,
            Linda::TupleEntryType::Int,
            Linda::TupleEntryType::Float
    };

    int i = 0;
    for (auto &p : pattern.getPatterns()) {
        EXPECT_EQ(p.getTupleType(), expectedTypes[i]);
        EXPECT_EQ(p.getType(), Linda::PatternEntryType::Any);
        i++;
    }
}

TEST(PatternTest, AnyExceptionsGetValue) {
    Linda::Pattern pattern;
    pattern.add<Linda::PatternEntryType::Any>(Linda::TupleEntryType::String);
    pattern.add<Linda::PatternEntryType::Any>(Linda::TupleEntryType::Int);
    pattern.add<Linda::PatternEntryType::Any>(Linda::TupleEntryType::Float);

    for (auto &p : pattern.getPatterns()) {
        try {
            p.getValue();
            FAIL() << "Expected AnyException";
        }
        catch (Linda::Exception::Pattern::AnyException const &err) {
            EXPECT_EQ(err.what(), std::string("PatternEntryType::Any doesn't have any value!"));
        }
        catch (...) {
            FAIL() << "Expected AnyException";
        }
    }
}

TEST(PatternTest, AnyExceptionsSetValue) {
    Linda::Pattern pattern;

    try {
        pattern.add<Linda::PatternEntryType::Any>(2);
        FAIL() << "Expected AnyException";
    }
    catch (Linda::Exception::Pattern::AnyException const &err) {
        EXPECT_EQ(err.what(), std::string("Can't use Any with specific value."));
    }
    catch (...) {
        FAIL() << "Expected AnyException";
    }

    try {
        pattern.add<Linda::PatternEntryType::Any>(2.0f);
        FAIL() << "Expected AnyException";
    }
    catch (Linda::Exception::Pattern::AnyException const &err) {
        EXPECT_EQ(err.what(), std::string("Can't use Any with specific value."));
    }
    catch (...) {
        FAIL() << "Expected AnyException";
    }

    try {
        pattern.add<Linda::PatternEntryType::Any>("test");
        FAIL() << "Expected AnyException";
    }
    catch (Linda::Exception::Pattern::AnyException const &err) {
        EXPECT_EQ(err.what(), std::string("Can't use Any with specific value."));
    }
    catch (...) {
        FAIL() << "Expected AnyException";
    }
}

TEST(PatternTest, Inequal) {
    Linda::Pattern pattern;
    Linda::PatternEntryType types[] = {
            // no Equal
            Linda::PatternEntryType::Less,
            Linda::PatternEntryType::LessOrEqual,
            Linda::PatternEntryType::Greater,
            Linda::PatternEntryType::GreaterOrEqual,
            // no Any
    };
    int arrSize = sizeof(types) / sizeof(Linda::PatternEntryType);
    const int expectedValTypes[] = {0, 1, 2};

    for (int i = 0; i < arrSize; i++) {
        pattern.add(types[i], 123);
        pattern.add(types[i], 2.0f);
        pattern.add(types[i], "Test");
    }

    int i = 0;
    int j = 0;
    for (auto &p : pattern.getPatterns()) {
        EXPECT_EQ(p.getValue().index(), expectedValTypes[i]);
        EXPECT_EQ(p.getType(), types[j]);
        i++;
        if (i == 3) {
            i = 0;
            j++;
        }
    }
}

TEST(PatternTest, Equal) {
    Linda::Pattern pattern;
    const int expectedValTypes[] = {0, 2};

    pattern.add(Linda::PatternEntryType::Equal, 123);
    pattern.add(Linda::PatternEntryType::Equal, "Test");


    int i = 0;
    for (auto &p : pattern.getPatterns()) {
        EXPECT_EQ(p.getValue().index(), expectedValTypes[i]);
        EXPECT_EQ(p.getType(), Linda::PatternEntryType::Equal);
        i++;
    }
}

TEST(PatternTest, Path) {
    Linda::Pattern pattern;
    pattern.add(Linda::PatternEntryType::Equal, 123);
    pattern.add(Linda::PatternEntryType::LessOrEqual, 2.1f);
    pattern.add(Linda::PatternEntryType::Equal, "Test");

    std::string expectedPath = "ifs";
    EXPECT_EQ(pattern.path(), expectedPath);

    pattern.add(Linda::PatternEntryType::Any, Linda::TupleEntryType::Int);
    pattern.add(Linda::PatternEntryType::Any, Linda::TupleEntryType::Float);
    pattern.add(Linda::PatternEntryType::Any, Linda::TupleEntryType::String);

    expectedPath = "ifsifs";
    EXPECT_EQ(pattern.path(), expectedPath);

    const std::string expectedPaths[] = {
            "ifsifs",
            "ifsif",
            "ifsi",
            "ifs"
    };

    int i = 0;
    for (const auto &path : pattern.all_paths()) {
        EXPECT_EQ(path, expectedPaths[i]);
        i++;
    }
}

TEST(PatternTest, SerializationDeserialization) {
    Linda::Pattern pattern;
    Linda::PatternEntryType typesWOEqual[] = {
            // no Equal
            Linda::PatternEntryType::Less,
            Linda::PatternEntryType::LessOrEqual,
            Linda::PatternEntryType::Greater,
            Linda::PatternEntryType::GreaterOrEqual,
            // no Any
    };
    int arrSize = sizeof(typesWOEqual) / sizeof(Linda::PatternEntryType);

    for (int i = 0; i < arrSize; i++) {
        pattern.add(typesWOEqual[i], 123);
        pattern.add(typesWOEqual[i], 2.1f);
        pattern.add(typesWOEqual[i], "Test");
    }

    pattern.add(Linda::PatternEntryType::Equal, 123);
    pattern.add(Linda::PatternEntryType::Equal, "Test");

    const unsigned char ExpectedSerialization[] = {
            128, 130, 134, 123, 0, 0, 0, 130, 131, 134, 102, 102, 6, 64, 131,
            132, 134, 84, 101, 115, 116, 132, 130, 135, 123, 0, 0, 0, 130, 131,
            135, 102, 102, 6, 64, 131, 132, 135, 84, 101, 115, 116, 132, 130,
            136, 123, 0, 0, 0, 130, 131, 136, 102, 102, 6, 64, 131, 132, 136,
            84, 101, 115, 116, 132, 130, 137, 123, 0, 0, 0, 130, 131, 137, 102,
            102, 6, 64, 131, 132, 137, 84, 101, 115, 116, 132, 130, 133, 123, 0,
            0, 0, 130, 132, 133, 84, 101, 115, 116, 132, 129
    };
    int i = 0;
    for (auto &c : pattern.serialize()) {
        EXPECT_EQ(static_cast<unsigned>(c), ExpectedSerialization[i]);
        i++;
    }

    Linda::Pattern pattern2(pattern.serialize());
    EXPECT_EQ(pattern.to_string(), pattern2.to_string());
}
