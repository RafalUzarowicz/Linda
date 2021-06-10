#include <gtest/gtest.h>
#include <iostream>
#include <variant>
#include <vector>
#include "Tuple.h"
#include "Pattern.h"
#include "Exceptions.h"


// TUPLE TESTS
TEST(TupleTest, Types) {
    Linda::Tuple t1 = Linda::Tuple();
    t1.push(4);
    ASSERT_EQ(t1[0].getType(), Linda::TupleEntryType::Int);
    t1.clear();
    t1.push(1.1f);
    ASSERT_EQ(t1[0].getType(), Linda::TupleEntryType::Float);
    t1.clear();
    t1.push("Test");
    ASSERT_EQ(t1[0].getType(), Linda::TupleEntryType::String);

    Linda::Tuple tuple(Linda::Tuple().push(2).push(3.0f).push("Tak"));
    const Linda::TupleEntryType expectedTypes[] = {
        Linda::TupleEntryType::Int, 
        Linda::TupleEntryType::Float, 
        Linda::TupleEntryType::String};
    
    int i = 0;
    for(auto& val : tuple){
        EXPECT_EQ(val.getType(), expectedTypes[i]);
        i++;
    }
}

TEST(TupleTest, Path) {
    Linda::Tuple t1 = Linda::Tuple();
    t1.push(4);
    ASSERT_EQ(t1.path(), "i");
    t1.clear();
    t1.push(1.1f);
    ASSERT_EQ(t1.path(), "f");
    t1.clear();
    t1.push("Test");
    ASSERT_EQ(t1.path(), "s");

    Linda::Tuple tuple(Linda::Tuple().push(2).push(3.0f).push("Tak"));
    EXPECT_EQ(tuple.path(), "ifs");
}

TEST(TupleTest, SerializationDeserialization) {
    Linda::Tuple t1 = Linda::Tuple();
    t1.push(4);
    const unsigned char ExpectedSerialization1[] = {128, 130, 4, 0, 0, 0, 130, 129};
    int i = 0;
    for(auto& c : t1.serialize()){
        ASSERT_EQ(static_cast<unsigned>(c), ExpectedSerialization1[i]);
        i++;
    }
    t1.clear();

    t1.push(3.0f);
    const unsigned char ExpectedSerialization2[] = {128, 131, 0, 0, 64, 64, 131, 129};
    i = 0;
    for(auto& c : t1.serialize()){
        ASSERT_EQ(static_cast<unsigned>(c), ExpectedSerialization2[i]);
        i++;
    }
    t1.clear();

    t1.push("Tak");
    const unsigned char ExpectedSerialization3[] = {128, 132, 84, 97, 107, 132, 129};
    i = 0;
    for(auto& c : t1.serialize()){
        ASSERT_EQ(static_cast<unsigned>(c), ExpectedSerialization3[i]);
        i++;
    }
    t1.clear();

    Linda::Tuple tuple(Linda::Tuple().push(2).push(3.0f).push("Tak"));
    const unsigned char ExpectedSerialization4[] = {
        128, 130, 2, 0, 0, 0, 130, 131, 0, 0, 64, 64, 131, 132, 84, 97, 107, 132, 129
    };
    i = 0;
    for(auto& c : tuple.serialize()){
        EXPECT_EQ(static_cast<unsigned>(c), ExpectedSerialization4[i]);
        i++;
    }

    Linda::Tuple tuple2(tuple.serialize());

    EXPECT_EQ(tuple.to_string(), tuple2.to_string());
}

TEST(TupleTest, SerializationDeserializationEmpty) {
    Linda::Tuple t1 = Linda::Tuple();
    const unsigned char ExpectedSerialization1[] = {128, 129};
    int i = 0;
    for(auto& c : t1.serialize()){
        ASSERT_EQ(static_cast<unsigned>(c), ExpectedSerialization1[i]);
        i++;
    }

    Linda::Tuple t2(t1.serialize());
    EXPECT_EQ(t1.to_string(), t2.to_string());
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
    for(auto& p : pattern.getPatterns()){
        EXPECT_EQ(p.getTupleType(),  expectedTypes[i]);
        EXPECT_EQ(p.getType(),  Linda::PatternEntryType::Any);
        i++;
    }
}

TEST(PatternTest, AnyExceptionsGetValue) {
    Linda::Pattern pattern;
    pattern.add<Linda::PatternEntryType::Any>(Linda::TupleEntryType::String);
    pattern.add<Linda::PatternEntryType::Any>(Linda::TupleEntryType::Int);
    pattern.add<Linda::PatternEntryType::Any>(Linda::TupleEntryType::Float);

    for(auto& p : pattern.getPatterns()){
        try {
            p.getValue();
            FAIL() << "Expected AnyException";
        }
        catch(Linda::Exception::Pattern::AnyException const & err) {
            EXPECT_EQ(err.what(), std::string("PatternEntryType::Any doesn't have any value!"));
        }
        catch(...) {
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
    catch(Linda::Exception::Pattern::AnyException const & err) {
        EXPECT_EQ(err.what(), std::string("Can't use Any with specific value."));
    }
    catch(...) {
        FAIL() << "Expected AnyException";
    }

    try {
        pattern.add<Linda::PatternEntryType::Any>(2.0f);
        FAIL() << "Expected AnyException";
    }
    catch(Linda::Exception::Pattern::AnyException const & err) {
        EXPECT_EQ(err.what(), std::string("Can't use Any with specific value."));
    }
    catch(...) {
        FAIL() << "Expected AnyException";
    }

    try {
        pattern.add<Linda::PatternEntryType::Any>("test");
        FAIL() << "Expected AnyException";
    }
    catch(Linda::Exception::Pattern::AnyException const & err) {
        EXPECT_EQ(err.what(), std::string("Can't use Any with specific value."));
    }
    catch(...) {
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
    int arrSize = sizeof(types)/sizeof(Linda::PatternEntryType);
    const int expectedValTypes[] = {0, 1, 2};
    
    for (int i=0; i<arrSize; i++) {
        pattern.add(types[i], 123);
        pattern.add(types[i], 2.0f);
        pattern.add(types[i], "Test");
    }
    
    int i = 0;
    int j = 0;
    for(auto& p : pattern.getPatterns()){
        EXPECT_EQ(p.getValue().index(),  expectedValTypes[i]);
        EXPECT_EQ(p.getType(),  types[j]);
        i++;
        if(i == 3){
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
    for(auto& p : pattern.getPatterns()){
        EXPECT_EQ(p.getValue().index(),  expectedValTypes[i]);
        EXPECT_EQ(p.getType(),  Linda::PatternEntryType::Equal);
        i++;
    }
}

TEST(PatternTest, Path) {
    Linda::Pattern p1 = Linda::Pattern();
    p1.add(Linda::PatternEntryType::Equal, 123);
    ASSERT_EQ(p1.path(), "i");
    p1.clear();
    p1.add(Linda::PatternEntryType::LessOrEqual, 2.1f);
    ASSERT_EQ(p1.path(), "f");
    p1.clear();
    p1.add(Linda::PatternEntryType::Equal, "Test");
    ASSERT_EQ(p1.path(), "s");
    p1.clear();


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
    for (const auto& path : pattern.all_paths()){
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
    int arrSize = sizeof(typesWOEqual)/sizeof(Linda::PatternEntryType);
    
    for (int i=0; i<arrSize; i++) {
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
    for(auto& c : pattern.serialize()){
        EXPECT_EQ(static_cast<unsigned>(c), ExpectedSerialization[i]);
        i++;
    }

    Linda::Pattern pattern2(pattern.serialize());
    EXPECT_EQ(pattern.to_string(), pattern2.to_string());
}

TEST(PatternTest, SerializationDeserializationEmpty) {
    Linda::Pattern p1 = Linda::Pattern();
    const unsigned char ExpectedSerialization1[] = {128, 129};
    int i = 0;
    for(auto& c : p1.serialize()){
        ASSERT_EQ(static_cast<unsigned>(c), ExpectedSerialization1[i]);
        i++;
    }

    Linda::Pattern p2(p1.serialize());
    EXPECT_EQ(p1.to_string(), p2.to_string());
}

TEST(PatternTest, checkSimple) {
    Linda::PatternEntryType types[] = {
        Linda::PatternEntryType::Equal,
        Linda::PatternEntryType::Less,
        Linda::PatternEntryType::LessOrEqual,
        Linda::PatternEntryType::Greater,
        Linda::PatternEntryType::GreaterOrEqual,
        Linda::PatternEntryType::Any
    };
    int arrSize = sizeof(types)/sizeof(Linda::PatternEntryType);
    Linda::Pattern p = Linda::Pattern();
    Linda::Tuple t = Linda::Tuple();

    const bool expectedResultsForEqual[] = {true, false, true, false, true, true};
    const bool expectedResultsForGreater[] = {false, false, false, true, true, true};
    const bool expectedResultsForLess[] = {false, true, true, false, false, true};

    // INT
    std::vector<Linda::Pattern> patternsForInt;
    for(int i=0; i<arrSize-1; i++){
        p.add(types[i], 123);
        patternsForInt.push_back(p);
        p.clear();
    }
    p.add(Linda::PatternEntryType::Any, Linda::TupleEntryType::Int);
    patternsForInt.push_back(p);
    p.clear();

    t.push(123);
    for(int i=0; i<arrSize; i++){
        EXPECT_EQ(patternsForInt.at(i).check(t), expectedResultsForEqual[i]);
    }
    t.clear();

    t.push(124);
    for(int i=0; i<arrSize; i++){
        EXPECT_EQ(patternsForInt.at(i).check(t), expectedResultsForGreater[i]);
    }
    t.clear();

    t.push(122);
    for(int i=0; i<arrSize; i++){
        EXPECT_EQ(patternsForInt.at(i).check(t), expectedResultsForLess[i]);
    }
    t.clear();

    // FLOAT
    std::vector<Linda::Pattern> patternsForFloat;
    for(int i=1; i<arrSize-1; i++){
        p.add(types[i], 5.0f);
        patternsForFloat.push_back(p);
        p.clear();
    }
    p.add(Linda::PatternEntryType::Any, Linda::TupleEntryType::Float);
    patternsForFloat.push_back(p);
    p.clear();

    t.push(5.0f);
    for(int i=1; i<arrSize; i++){
        EXPECT_EQ(patternsForFloat.at(i-1).check(t), expectedResultsForEqual[i]);
    }
    t.clear();

    t.push(5.1f);
    for(int i=1; i<arrSize; i++){
        EXPECT_EQ(patternsForFloat.at(i-1).check(t), expectedResultsForGreater[i]);
    }
    t.clear();

    t.push(4.9f);
    for(int i=1; i<arrSize; i++){
        EXPECT_EQ(patternsForFloat.at(i-1).check(t), expectedResultsForLess[i]);
    }
    t.clear();

    // STRING
    std::vector<Linda::Pattern> patternsForString;
    for(int i=0; i<arrSize-1; i++){
        p.add(types[i], "ccc");
        patternsForString.push_back(p);
        p.clear();
    }
    p.add(Linda::PatternEntryType::Any, Linda::TupleEntryType::String);
    patternsForString.push_back(p);
    p.clear();

    t.push("ccc");
    for(int i=0; i<arrSize; i++){
        EXPECT_EQ(patternsForString.at(i).check(t), expectedResultsForEqual[i]);
    }
    t.clear();

    t.push("ccd");
    for(int i=0; i<arrSize; i++){
        EXPECT_EQ(patternsForString.at(i).check(t), expectedResultsForGreater[i]);
    }
    t.clear();

    t.push("cca");
    for(int i=0; i<arrSize; i++){
        EXPECT_EQ(patternsForString.at(i).check(t), expectedResultsForLess[i]);
    }
    t.clear();
}

TEST(PatternTest, checkComplex) {
    Linda::Pattern p = Linda::Pattern();
    Linda::Tuple t0 = Linda::Tuple();
    EXPECT_TRUE(p.check(t0));
    p.add(Linda::PatternEntryType::Equal, 100);
    p.add(Linda::PatternEntryType::GreaterOrEqual, 4.0f);
    p.add(Linda::PatternEntryType::Less, "ddd");
    Linda::Tuple t1(Linda::Tuple().push(100).push(4.0f).push("aaa"));
    Linda::Tuple t2(Linda::Tuple().push(100).push(3.0f).push("aaa"));
    Linda::Tuple t3(Linda::Tuple().push(100).push(4.0f));
    Linda::Tuple t4(Linda::Tuple().push(100).push(5.0f).push("aaa").push(1));
    EXPECT_TRUE(p.check(t1));
    EXPECT_FALSE(p.check(t2));
    EXPECT_FALSE(p.check(t3));
    EXPECT_FALSE(p.check(t4));
    p.add(Linda::PatternEntryType::Any, Linda::TupleEntryType::Int);
    p.add(Linda::PatternEntryType::Any, Linda::TupleEntryType::Float);
    p.add(Linda::PatternEntryType::Any, Linda::TupleEntryType::String);
    EXPECT_TRUE(p.check(t1));
    EXPECT_TRUE(p.check(t4));
    p.add(Linda::PatternEntryType::Less, 100);
    EXPECT_FALSE(p.check(t1));
    EXPECT_FALSE(p.check(t4));
    t4.push(6.0f).push("test").push(99);
    EXPECT_TRUE(p.check(t4));
}
