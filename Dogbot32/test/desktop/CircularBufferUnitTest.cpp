#include "gtest/gtest.h"
#include "CircularBuffer.h"

float buf[10];

class MyClass {
    
    CircularBuffer<float> cb = CircularBuffer<float>(buf, 10);

public:
    MyClass() {};

};

TEST(CircularBufferTestSuite, ConstructorTest) {
    int buf[5];
    CircularBuffer<int> cb(buf, 5);

    EXPECT_TRUE(cb.isEmpty());
    EXPECT_FALSE(cb.isFull());
    EXPECT_EQ(cb.getSize(), 0);
    EXPECT_EQ(cb.getCapacity(), 5);

    for (int i = 0; i < 20; i++) {
        cb.addElement(i);
    }

    EXPECT_FALSE(cb.isEmpty());
    EXPECT_TRUE(cb.isFull());

    for (int i = 0; i < 4; i++) {
        cb.getElement();
    }

    EXPECT_FALSE(cb.isEmpty());
    EXPECT_FALSE(cb.isFull());
    EXPECT_EQ(cb.getSize(), 1);
}