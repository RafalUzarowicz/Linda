//
// Created by Łukasz Pokorzyński on 09.06.2021.
//

#include <gtest/gtest.h>
#include "Signals.h"
#include "Linda.h"

TEST(SignalTest, Encoding) {
    int depth = 7, index = 4;
    int newEncodedValue = Linda::Signal::encode(depth, index);

    ASSERT_EQ(newEncodedValue, 39);
}

TEST(SignalTest, EncodingThrow) {
    int depth = 8, index = 4;
    int newEncodedValue;

    try {
        newEncodedValue = Linda::Signal::encode(depth, index);
        FAIL() << "Expected EncodingException";
    }
    catch(Linda::Exception::Signal::EncodingException const & err) {
        EXPECT_EQ(err.what(), std::string("Too big depth value: 8"));
    }
    catch(...) {
        FAIL() << "Expected AnyException";
    }
}

TEST(SignalTest, Decoding) {
    int depth = 7, index = 4;
    int newEncodedValue = Linda::Signal::encode(depth, index);
    int extractedDepth, extractedIndex;
    Linda::Signal::decode(newEncodedValue, extractedDepth, extractedIndex);

    ASSERT_EQ(newEncodedValue, 39);
    ASSERT_EQ(extractedDepth, 7);
    ASSERT_EQ(extractedIndex, 4);
}

TEST(SignalTest, Sending) {
    pid_t pid;

    if((pid = fork()) < 0) {
        FAIL() << "Could not create child process";
    } else if(pid == 0) {
        registerHandler();
        pause();

        ASSERT_NO_THROW();
    } else {
        sleep(1);
        Linda::Signal::notify(pid, 7, 4);
    }
}
