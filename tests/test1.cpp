#include <gtest/gtest.h>
#include <iostream>
#include "Tuple.h"
#include "Pattern.h"


TEST(TupleTest, TypesTest) {
    Linda::Tuple tuple(Linda::Tuple().push(2).push(3.0f).push("Tak"));
    const Linda::TupleEntryType expectedTypes[] = {Linda::TupleEntryType::Int, Linda::TupleEntryType::Float, Linda::TupleEntryType::String};
    
    int i = 0;
    for(auto& val : tuple){
        EXPECT_EQ(val.getType(), expectedTypes[i]);
        i++;
    }
}

TEST(TupleTest, PathTest) {
    Linda::Tuple tuple(Linda::Tuple().push(2).push(3.0f).push("Tak"));
    const std::string path = "ifs";

    EXPECT_EQ(tuple.path(), path);
}

TEST(TupleTest, SerializationDeserialization) {
    Linda::Tuple tuple(Linda::Tuple().push(2).push(3.0f).push("Tak"));
    const unsigned char ExpectedSerialization[] = {128, 130, 2, 0, 0, 0, 130, 131, 0, 0, 64, 64, 131, 132, 84, 97, 107, 132, 129};

    int i = 0;
    for(auto& c : tuple.serialize()){
        EXPECT_EQ(static_cast<unsigned>(c), ExpectedSerialization[i]);
        i++;
    }

    Linda::Tuple tuple2(tuple.serialize());

    EXPECT_EQ(tuple.to_string(), tuple2.to_string());
}