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
    Linda::create();
    Linda::connect();

    Linda::Tuple t1;
    Linda::Pattern p1;
    p1.add(Linda::PatternEntryType::Equal, 12);
    t1.push(12);
    Linda::output(t1);
    ASSERT_EQ(find(p1, "tuplespace/i.linda", false).to_string(), t1.to_string());

    Linda::Tuple t2;
    Linda::Pattern p2;
    p2.add(Linda::PatternEntryType::GreaterOrEqual, 4.0f);
    t2.push(4.0f);
    Linda::output(t2);
    ASSERT_EQ(find(p2, "tuplespace/f.linda", false).to_string(), t2.to_string());

    Linda::Tuple t3;
    Linda::Pattern p3;
    p3.add(Linda::PatternEntryType::Equal, "Tak");
    t3.push("Tak");
    Linda::output(t3);
    ASSERT_EQ(find(p3, "tuplespace/s.linda", false).to_string(), t3.to_string());

    Linda::Tuple t4;
    Linda::Pattern p4;
    p4.add(Linda::PatternEntryType::Equal, 12);
    p4.add(Linda::PatternEntryType::GreaterOrEqual, 4.0f);
    p4.add(Linda::PatternEntryType::Equal, "Tak");
    t4.push(12);
    t4.push(4.0f);
    t4.push("Tak");
    Linda::output(t4);
    ASSERT_EQ(find(p4, "tuplespace/ifs.linda", false).to_string(), t4.to_string());

    Linda::Tuple t5;
    Linda::Pattern p5;
    p5.add(Linda::PatternEntryType::Equal, 12);
    p5.add(Linda::PatternEntryType::GreaterOrEqual, 4.0f);
    p5.add(Linda::PatternEntryType::Equal, "Nie");
    t5.push(12);
    t5.push(4.0f);
    t5.push("Nie");
    Linda::output(t5);
    ASSERT_EQ(find(p5, "tuplespace/ifs.linda", false).to_string(), t5.to_string());
}

TEST(OutputTest, OutputTooBigTuple) {
    Linda::create();
    Linda::connect();

    Linda::Tuple t;
    for(int i = 0; i < 8; i++)
        t.push("testestestestestestestestsetsetsetestestestestestesttestestestestestestest");

    ASSERT_THROW(Linda::output(t), Linda::Exception::TupleSpaceException);
}

TEST(OutputTest, OutputEmpty) {
    Linda::create();
    Linda::connect();

    Linda::Tuple t;
    ASSERT_THROW(Linda::output(t), Linda::Exception::TupleSpaceException);
}

TEST(ReadTest, SimpleRead) {
    Linda::create();
    Linda::connect();

    Linda::Pattern p1;
    p1.add(Linda::PatternEntryType::Equal,12);
    ASSERT_EQ(Linda::read(p1).to_string(), "(i:12)");

    Linda::Pattern p2;
    p2.add(Linda::PatternEntryType::GreaterOrEqual,4.0f);
    ASSERT_EQ(Linda::read(p2).to_string(), "(f:4.0)");

    Linda::Pattern p3;
    p3.add(Linda::PatternEntryType::Equal,"Tak");
    ASSERT_EQ(Linda::read(p3).to_string(), "(s:\"Tak\")");

    Linda::Pattern p4;
    p4.add(Linda::PatternEntryType::Equal, 12);
    p4.add(Linda::PatternEntryType::GreaterOrEqual, 4.0f);
    p4.add(Linda::PatternEntryType::Equal, "Tak");
    ASSERT_EQ(Linda::read(p4).to_string(), "(i:12, f:4.0, s:\"Tak\")");
}

TEST(ReadTest, AnyRead) {
    Linda::create();
    Linda::connect();

    Linda::Pattern p1;
    p1.add(Linda::PatternEntryType::Equal,12);
    p1.add(Linda::PatternEntryType::GreaterOrEqual, 4.0f);
    p1.add<Linda::PatternEntryType::Any>(Linda::TupleEntryType::String);
    ASSERT_EQ(Linda::read(p1).to_string(), "(i:12, f:4.0, s:\"Tak\")");
}

TEST(ReadTest, ReadTooBigPattern) {
    Linda::create();
    Linda::connect();

    Linda::Pattern p;
    for(int i = 0; i < 8; i++)
        p.add<Linda::PatternEntryType::Equal>("testestestestestestestestsetsetsetestestestestestesttestestestestestestest");

    ASSERT_THROW(Linda::read(p), Linda::Exception::TupleSpaceException);
}

TEST(ReadTest, ReadEmpty) {
    Linda::create();
    Linda::connect();

    Linda::Pattern p;
    ASSERT_THROW(Linda::read(p), Linda::Exception::TupleSpaceException);
}

TEST(InputTest, SimpleInput) {
    Linda::create();
    Linda::connect();

    Linda::Pattern right;
    right.add(Linda::PatternEntryType::Equal,12);
    ASSERT_EQ(Linda::input(right).to_string(), "(i:12)");

    Linda::Pattern p2;
    p2.add(Linda::PatternEntryType::GreaterOrEqual,4.0f);
    ASSERT_EQ(Linda::input(p2).to_string(), "(f:4.0)");

    Linda::Pattern p3;
    p3.add(Linda::PatternEntryType::Equal,"Tak");
    ASSERT_EQ(Linda::input(p3).to_string(), "(s:\"Tak\")");

    Linda::Pattern p4;
    p4.add(Linda::PatternEntryType::Equal, 12);
    p4.add(Linda::PatternEntryType::GreaterOrEqual, 4.0f);
    p4.add(Linda::PatternEntryType::Equal, "Tak");
    ASSERT_EQ(Linda::input(p4).to_string(), "(i:12, f:4.0, s:\"Tak\")");
}

TEST(InputTest, AnyInput) {
    Linda::create();
    Linda::connect();

    Linda::Pattern p1;
    p1.add(Linda::PatternEntryType::Equal,12);
    p1.add(Linda::PatternEntryType::GreaterOrEqual, 4.0f);
    p1.add<Linda::PatternEntryType::Any>(Linda::TupleEntryType::String);
    ASSERT_EQ(Linda::read(p1).to_string(), "(i:12, f:4.0, s:\"Nie\")");
}

TEST(InputTest, InputTooBigPattern) {
    Linda::create();
    Linda::connect();

    Linda::Pattern p;
    for(int i = 0; i < 8; i++)
        p.add<Linda::PatternEntryType::Equal>("testestestestestestestestsetsetsetestestestestestesttestestestestestestest");

    ASSERT_THROW(Linda::input(p), Linda::Exception::TupleSpaceException);
}

TEST(InputTest, InputEmpty) {
    Linda::create();
    Linda::connect();

    Linda::Pattern p;
    ASSERT_THROW(Linda::input(p), Linda::Exception::TupleSpaceException);
}
