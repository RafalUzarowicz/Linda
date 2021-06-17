//
// Created by Łukasz Pokorzyński on 10.06.2021.
//

#include <gtest/gtest.h>
#include "Tuple.h"
#include "Pattern.h"
#include "Exceptions.h"
#include "TupleSpace.h"
#include "Linda.h"

TEST(OutputTest, Output) {
    Linda::create("tests/output");
    Linda::connect("tests/output/tuplespace");

    Linda::Tuple t1;
    Linda::Pattern p1;
    p1.add(Linda::PatternEntryType::Equal, 12);
    t1.push(12);
    Linda::output(t1);
    ASSERT_EQ(find(p1, "tests/output/tuplespace/i.linda", false).to_string(), t1.to_string());

    Linda::Tuple t2;
    Linda::Pattern p2;
    p2.add(Linda::PatternEntryType::GreaterOrEqual, 4.0f);
    t2.push(4.0f);
    Linda::output(t2);
    ASSERT_EQ(find(p2, "tests/output/tuplespace/f.linda", false).to_string(), t2.to_string());

    Linda::Tuple t3;
    Linda::Pattern p3;
    p3.add(Linda::PatternEntryType::Equal, "Tak");
    t3.push("Tak");
    Linda::output(t3);
    ASSERT_EQ(find(p3, "tests/output/tuplespace/s.linda", false).to_string(), t3.to_string());

    Linda::Tuple t4;
    Linda::Pattern p4;
    p4.add(Linda::PatternEntryType::Equal, 12);
    p4.add(Linda::PatternEntryType::GreaterOrEqual, 4.0f);
    p4.add(Linda::PatternEntryType::Equal, "Tak");
    t4.push(12);
    t4.push(4.0f);
    t4.push("Tak");
    Linda::output(t4);
    ASSERT_EQ(find(p4, "tests/output/tuplespace/ifs.linda", false).to_string(), t4.to_string());

    Linda::Tuple t5;
    Linda::Pattern p5;
    p5.add(Linda::PatternEntryType::Equal, 12);
    p5.add(Linda::PatternEntryType::GreaterOrEqual, 4.0f);
    p5.add(Linda::PatternEntryType::Equal, "Nie");
    t5.push(12);
    t5.push(4.0f);
    t5.push("Nie");
    Linda::output(t5);
    ASSERT_EQ(find(p5, "tests/output/tuplespace/ifs.linda", false).to_string(), t5.to_string());
}

TEST(OutputTest, OutputTooBigTuple) {
    Linda::create("tests/OutputTooBigTuple");
    Linda::connect("tests/OutputTooBigTuple/tuplespace");

    Linda::Tuple t;
    for(int i = 0; i < 8; i++)
        t.push("testestestestestestestestsetsetsetestestestestestesttestestestestestestest");

    ASSERT_THROW(Linda::output(t), Linda::Exception::TupleSpaceException);
}

TEST(OutputTest, OutputEmpty) {
    Linda::create("tests/OutputEmpty");
    Linda::connect("tests/OutputEmpty/tuplespace");

    Linda::Tuple t;
    ASSERT_THROW(Linda::output(t), Linda::Exception::TupleSpaceException);
}

TEST(ReadTest, SimpleRead) {
    Linda::create("tests/SimpleRead");
    Linda::connect("tests/SimpleRead/tuplespace");

    Linda::Tuple t1;
    t1.push(12);
    Linda::output(t1);

    Linda::Pattern p1;
    p1.add(Linda::PatternEntryType::Equal,12);
    ASSERT_EQ(Linda::read(p1).to_string(), "(i:12)");

    Linda::Tuple t2;
    t2.push(4.0f);
    Linda::output(t2);

    Linda::Pattern p2;
    p2.add(Linda::PatternEntryType::GreaterOrEqual,4.0f);
    ASSERT_EQ(Linda::read(p2).to_string(), "(f:4.0)");

    Linda::Tuple t3;
    t3.push("Tak");
    Linda::output(t3);

    Linda::Pattern p3;
    p3.add(Linda::PatternEntryType::Equal,"Tak");
    ASSERT_EQ(Linda::read(p3).to_string(), "(s:\"Tak\")");

    Linda::Tuple t4;
    t4.push(12).push(4.0f).push("Tak");
    Linda::output(t4);

    Linda::Pattern p4;
    p4.add(Linda::PatternEntryType::Equal, 12);
    p4.add(Linda::PatternEntryType::GreaterOrEqual, 4.0f);
    p4.add(Linda::PatternEntryType::Equal, "Tak");
    ASSERT_EQ(Linda::read(p4).to_string(), "(i:12, f:4.0, s:\"Tak\")");
}

TEST(ReadTest, AnyRead) {
    Linda::create("tests/AnyRead");
    Linda::connect("tests/AnyRead/tuplespace");

    Linda::Tuple t1;
    t1.push(12).push(4.0f).push("Nie");
    Linda::output(t1);

    Linda::Pattern p1;
    p1.add(Linda::PatternEntryType::Equal,12);
    p1.add(Linda::PatternEntryType::GreaterOrEqual, 4.0f);
    p1.add<Linda::PatternEntryType::Any>(Linda::TupleEntryType::String);
    ASSERT_TRUE(p1.check(Linda::read(p1)));
}

TEST(ReadTest, ReadTooBigPattern) {
    Linda::create("tests/ReadTooBigPattern");
    Linda::connect("tests/ReadTooBigPattern/tuplespace");

    Linda::Pattern p;
    for(int i = 0; i < 8; i++)
        p.add<Linda::PatternEntryType::Equal>("testestestestestestestestsetsetsetestestestestestesttestestestestestestest");

    ASSERT_THROW(Linda::read(p), Linda::Exception::TupleSpaceException);
}

TEST(ReadTest, ReadEmpty) {
    Linda::create("tests/ReadEmpty");
    Linda::connect("tests/ReadEmpty/tuplespace");

    Linda::Pattern p;
    ASSERT_THROW(Linda::read(p), Linda::Exception::TupleSpaceException);
}

TEST(InputTest, SimpleInput) {
    Linda::create("tests/SimpleInput");
    Linda::connect("tests/SimpleInput/tuplespace");

    Linda::Tuple t1;
    t1.push(12);
    Linda::output(t1);

    Linda::Pattern right;
    right.add(Linda::PatternEntryType::Equal,12);
    ASSERT_EQ(Linda::input(right).to_string(), "(i:12)");

    Linda::Tuple t2;
    t2.push(4.0f);
    Linda::output(t2);

    Linda::Pattern p2;
    p2.add(Linda::PatternEntryType::GreaterOrEqual,4.0f);
    ASSERT_EQ(Linda::input(p2).to_string(), "(f:4.0)");

    Linda::Tuple t3;
    t3.push("Tak");
    Linda::output(t3);

    Linda::Pattern p3;
    p3.add(Linda::PatternEntryType::Equal,"Tak");
    ASSERT_EQ(Linda::input(p3).to_string(), "(s:\"Tak\")");

    Linda::Tuple t4;
    t4.push(12).push(4.0f).push("Tak");
    Linda::output(t4);

    Linda::Pattern p4;
    p4.add(Linda::PatternEntryType::Equal, 12);
    p4.add(Linda::PatternEntryType::GreaterOrEqual, 4.0f);
    p4.add(Linda::PatternEntryType::Equal, "Tak");
    ASSERT_EQ(Linda::input(p4).to_string(), "(i:12, f:4.0, s:\"Tak\")");
}

TEST(InputTest, AnyInput) {
    Linda::create("tests/AnyInput");
    Linda::connect("tests/AnyInput/tuplespace");

    Linda::Tuple t1;
    t1.push(12).push(4.0f).push("Tak");
    Linda::output(t1);

    Linda::Pattern p1;
    p1.add(Linda::PatternEntryType::Equal,12);
    p1.add(Linda::PatternEntryType::GreaterOrEqual, 4.0f);
    p1.add<Linda::PatternEntryType::Any>(Linda::TupleEntryType::String);
    ASSERT_TRUE(p1.check(Linda::input(p1)));
}

TEST(InputTest, InputTooBigPattern) {
    Linda::create("tests/InputTooBigPattern");
    Linda::connect("tests/InputTooBigPattern/tuplespace");

    Linda::Pattern p;
    for(int i = 0; i < 8; i++)
        p.add<Linda::PatternEntryType::Equal>("testestestestestestestestsetsetsetestestestestestesttestestestestestestest");

    ASSERT_THROW(Linda::input(p), Linda::Exception::TupleSpaceException);
}

TEST(InputTest, InputEmpty) {
    Linda::create("tests/InputEmpty");
    Linda::connect("tests/InputEmpty/tuplespace");

    Linda::Pattern p;
    ASSERT_THROW(Linda::input(p), Linda::Exception::TupleSpaceException);
}
